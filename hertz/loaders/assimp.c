#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <hz/logger.h>
#include <stdlib.h>
#include <assert.h>

#include <vendor/stb/stb_image.h>
#include <vendor/linmath/linmath.h>
#include <hz/objects/mesh.h>
#include <hz/loader.h>

static const char *tag = "aimp";

/* Make sure we can copy assimp buffers directly */
static_assert(sizeof(ai_real) == sizeof(float));
static_assert(sizeof(struct aiMatrix4x4) == sizeof(mat4x4));

extern int assimp_shader;

static struct hz_mesh *mesh_new(GLuint program, struct hz_material *m)
{
	struct hz_mesh *o;
	bool ok;

	o = calloc(1, sizeof(struct hz_mesh));
	if (!o) {
		return NULL;
	}

	ok = hz_mesh_init(o, program, m);
	if (!ok) {
		free(o);
		return NULL;
	}

	return o;
}

static struct hz_material *material_new(GLuint program)
{
	struct hz_material *o;
	bool ok;

	o = calloc(1, sizeof(struct hz_material));
	if (!o) {
		return NULL;
	}

	ok = hz_material_init(o, program);
	if (!ok) {
		free(o);
		return NULL;
	}

	return o;
}

static void mesh_free(struct hz_mesh *o)
{
	hz_mesh_deinit(o);
	free(o);
}

static bool bind_aimp_texture(struct hz_material *m, void *buffer, size_t len)
{
	int w, h, n;
	uint8_t *data;
	bool ok;
	GLenum format;

	data = stbi_load_from_memory(buffer, len, &w, &h, &n, 0);
	if (!data) {
		return 0;
	}

	switch (n) {
	case 3:
		format = GL_RGB;
		break;
	case 4:
		format = GL_RGBA;
		break;
	default:
		ok = false;
		goto cleanup;
	}

	ok = hz_material_bind_texture(m, HZ_TEXTURE_DIFFUSE, data, format, w, h);

cleanup:
	stbi_image_free(data);
	return ok;
}

static struct aiTexture *ai_get_texture(struct aiMaterial *material,
		const struct aiScene *ai_scene, enum aiTextureType type, unsigned index)
{
	aiReturn ret;
	struct aiString path;
	int itexture;

	ret = aiGetMaterialString(material, AI_MATKEY_TEXTURE(type, index), &path);
	if (ret != AI_SUCCESS) {
		return NULL;
	}

	if (path.data[0] != '*') {
		hz_log_e(tag, "mesh is not using embedded texture, skipping");
		return NULL;
	}

	itexture = atoi(path.data + 1);
	return ai_scene->mTextures[itexture];
}

static bool apply_textures(struct hz_material *m, struct aiMesh *ai_mesh,
		const struct aiScene *ai_scene)
{
	struct aiMaterial *material;
	struct aiTexture *diffuse;

	material = ai_scene->mMaterials[ai_mesh->mMaterialIndex];
	if (!material) {
		return false;
	}

	diffuse = ai_get_texture(material, ai_scene, aiTextureType_DIFFUSE, 0);
	if (!diffuse) {
		return false;
	}

	return bind_aimp_texture(m, diffuse->pcData, diffuse->mWidth);
}

static bool attach_geometry(struct hz_mesh *o, struct aiMesh *ai_mesh)
{
	const size_t nindices = ai_mesh->mNumFaces * ai_mesh->mFaces[0].mNumIndices;
	unsigned *indices;
	bool ok;

	indices = calloc(1, sizeof(unsigned) * nindices);
	if (!indices) {
		return false;
	}

	size_t nindex = 0;
	for (size_t iface = 0; iface < ai_mesh->mNumFaces; iface++) {
		struct aiFace *face = &ai_mesh->mFaces[iface];

		for (size_t iindex = 0; iindex < face->mNumIndices; iindex++) {
			indices[nindex] = face->mIndices[iindex];
			nindex++;
		}
	}

	ok = hz_mesh_set_geometry(o,
		(vec3 *)ai_mesh->mVertices, (vec3 *)ai_mesh->mNormals,
		ai_mesh->mNumVertices,
		(vec3 *)ai_mesh->mTextureCoords[0], ai_mesh->mNumVertices,
		indices, nindices);

	free(indices);

	return ok;
}

static bool import_ai_mesh(struct hz_object *parent, struct aiMesh *ai_mesh,
		struct aiMatrix4x4 *ai_model, const struct aiScene *ai_scene)
{
	bool ok;
	struct hz_mesh *o;
	struct hz_material *m;

	m = material_new(assimp_shader);
	if (!m) {
		return false;
	}

	o = mesh_new(assimp_shader, m);
	if (!o) {
		return false;
	}

	ok = attach_geometry(o, ai_mesh);
	if (!ok) {
		mesh_free(o);
		return false;
	}

	ok = apply_textures(m, ai_mesh, ai_scene);
	if (!ok) {
		/* not an error */
		hz_log_e(tag, "unable to apply textures for mesh '%s'", ai_mesh->mName.data);
	}

	mat4x4_transpose(o->super.model, (void *)ai_model);
	hz_object_insert(parent, HZ_OBJECT(o));
	return true;
}

static void load_scene_node(struct hz_object *object, const struct aiScene *scene,
		struct aiNode *node)
{
	struct aiMesh *mesh;
	size_t mesh_index;

	for (size_t i = 0; i < node->mNumMeshes; i++) {
		mesh_index = node->mMeshes[i];
		mesh = scene->mMeshes[mesh_index];

		hz_log_i(tag, "loading mesh '%s'", mesh->mName.data);
		if (!import_ai_mesh(object, mesh, &node->mTransformation, scene)) {
			hz_log_e(tag, "unable to load mesh '%s'", mesh->mName.data);
		}
	}
}

static void load_scene_recursive(struct hz_object *object,
		const struct aiScene *scene, struct aiNode *node)
{
	struct aiNode *child;

	load_scene_node(object, scene, node);

	for (size_t i = 0; i < node->mNumChildren; i++) {
		child = node->mChildren[i];
		load_scene_recursive(object, scene, child);
	}
}

static bool assimp_import_scene(const char *path, struct hz_object *root)
{
	const struct aiScene *scene;
	unsigned int flags = aiProcess_FlipUVs | aiProcess_Triangulate
		| aiProcess_JoinIdenticalVertices;

	scene = aiImportFile(path, flags);
	if (!scene) {
		return false;
	}

	load_scene_recursive(root, scene, scene->mRootNode);
	aiReleaseImport(scene);

	return true;
}

static struct hz_loader_proto assimp_loader_proto = {
	.import_scene = assimp_import_scene,
};

hz_export_loader_proto(assimp_loader_proto);
