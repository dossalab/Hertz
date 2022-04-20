#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <hz/logger.h>
#include <stdlib.h>
#include <assert.h>

#include <vendor/stb/stb_image.h>

#include <hz/helpers/textures.h>
#include <hz/objects/basic.h>
#include <hz/scene.h>
#include <hz/loader.h>

static const char *tag = "aimp";

/* Make sure we can copy assimp buffers directly */
static_assert(sizeof(ai_real) == sizeof(float));
static_assert(sizeof(struct aiMatrix4x4) == sizeof(mat4x4));

extern int assimp_shader;

static struct hz_basic_object *basic_object_new(GLuint program)
{
	struct hz_basic_object *o;
	bool ok;

	o = calloc(1, sizeof(struct hz_basic_object));
	if (!o) {
		return NULL;
	}

	ok = hz_object_init(&o->as_object, program, &hz_basic_object_proto);
	if (!ok) {
		free(o);
		return NULL;
	}

	return o;
}

static void basic_object_free(struct hz_basic_object *o)
{
	hz_object_deinit(&o->as_object);
	free(o);
}

static inline bool ai_get_texture_helper(struct aiMaterial *material,
		enum aiTextureType type, unsigned index, struct aiString *path)
{
	aiReturn ret;

	ret = aiGetMaterialString(material, AI_MATKEY_TEXTURE(type, index), path);
	return ret == AI_SUCCESS;
}


static GLuint create_texture_from_memory(void *buffer, size_t len)
{
	int w, h, n;
	uint8_t *data;
	GLuint texture;

	data = stbi_load_from_memory(buffer, len, &w, &h, &n, 0);
	if (!data) {
		return 0;
	}

	switch (n) {
	case 3:
		texture = hz_create_texture(data, GL_RGB, w, h);
		break;
	case 4:
		texture = hz_create_texture(data, GL_RGBA, w, h);
		break;
	default:
		texture = 0;
		break;
	}

	stbi_image_free(data);
	return texture;
}

static GLuint create_texture_from_mesh(struct aiMesh *ai_mesh,
		const struct aiScene *ai_scene)
{
	struct aiMaterial *material;
	struct aiString path;
	struct aiTexture *texture;
	bool ok;
	int texture_index;

	material = ai_scene->mMaterials[ai_mesh->mMaterialIndex];
	if (!material) {
		return 0;
	}

	ok = ai_get_texture_helper(material, aiTextureType_DIFFUSE, 0, &path);
	if (!ok) {
		return 0;
	}

	if (path.data[0] != '*') {
		hz_log_e(tag, "mesh is not using embedded texture, skipping");
		return 0;
	}

	texture_index = atoi(path.data + 1);
	texture = ai_scene->mTextures[texture_index];

	return create_texture_from_memory(texture->pcData, texture->mWidth);
}

static bool apply_textures(struct hz_basic_object *o, struct aiMesh *ai_mesh,
		const struct aiScene *ai_scene)
{
	GLuint texture;

	texture = create_texture_from_mesh(ai_mesh, ai_scene);
	if (!texture) {
		return false;
	}

	hz_basic_object_set_texture(o, texture, (vec3 *)ai_mesh->mTextureCoords[0],
			ai_mesh->mNumVertices);

	return true;
}

static bool attach_geometry(struct hz_basic_object *o, struct aiMesh *ai_mesh)
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

	ok = hz_basic_object_set_geometry(o,
		(vec3 *)ai_mesh->mVertices, (vec3 *)ai_mesh->mNormals,
		ai_mesh->mNumVertices,
		indices, nindices);

	free(indices);

	return ok;
}

static bool import_ai_mesh(struct hz_scene *s, struct aiMesh *ai_mesh,
		struct aiMatrix4x4 *ai_model, const struct aiScene *ai_scene)
{
	bool ok;
	struct hz_basic_object *o;

	o = basic_object_new(assimp_shader);
	if (!o) {
		return false;
	}

	ok = attach_geometry(o, ai_mesh);
	if (!ok) {
		basic_object_free(o);
		return false;
	}

	ok = apply_textures(o, ai_mesh, ai_scene);
	if (!ok) {
		/* not an error */
		hz_log_e(tag, "unable to apply textures for mesh '%s'", ai_mesh->mName.data);
	}

	mat4x4_transpose(o->transform.model, (void *)ai_model);
	hz_scene_attach(s, &o->as_object);
	return true;
}

static void load_scene_node(struct hz_scene *s, const struct aiScene *scene,
		struct aiNode *node)
{
	struct aiMesh *mesh;
	size_t mesh_index;

	for (size_t i = 0; i < node->mNumMeshes; i++) {
		mesh_index = node->mMeshes[i];
		mesh = scene->mMeshes[mesh_index];

		hz_log_i(tag, "loading mesh '%s'", mesh->mName.data);
		if (!import_ai_mesh(s, mesh, &node->mTransformation, scene)) {
			hz_log_e(tag, "unable to load mesh '%s'", mesh->mName.data);
		}
	}
}

static void load_scene_recursive(struct hz_scene *s, const struct aiScene *scene,
		struct aiNode *node)
{
	struct aiNode *child;

	load_scene_node(s, scene, node);

	for (size_t i = 0; i < node->mNumChildren; i++) {
		child = node->mChildren[i];
		load_scene_recursive(s, scene, child);
	}
}

static bool assimp_import_scene(const char *path, struct hz_scene *s)
{
	const struct aiScene *scene;
	unsigned int flags = aiProcess_FlipUVs | aiProcess_Triangulate
		| aiProcess_JoinIdenticalVertices;

	hz_scene_init(s);

	scene = aiImportFile(path, flags);
	if (!scene) {
		return false;
	}

	load_scene_recursive(s, scene, scene->mRootNode);
	aiReleaseImport(scene);

	return true;
}

static struct hz_loader_proto assimp_loader_proto = {
	.import_scene = assimp_import_scene,
};

hz_export_loader_proto(assimp_loader_proto);
