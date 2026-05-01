#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <hz/util/logger.h>
#include <assert.h>
#include <hz/types.h>
#include <hz/node.h>
#include <hz/nodes/mesh.h>
#include <hz/nodes/root.h>
#include <hz/material.h>
#include <hz/material_store.h>

#include <examples/vendor/stb/stb_image.h>

static const char *tag = "aimp";

/* Make sure we can copy assimp buffers directly */
static_assert(sizeof(ai_real) == sizeof(float), "ai_real is not float");

static void material_key(const struct aiScene *ai_scene, unsigned mat_index,
		char *buf, size_t size)
{
	struct aiString name;
	struct aiMaterial *ai_mat = ai_scene->mMaterials[mat_index];

	if (aiGetMaterialString(ai_mat, AI_MATKEY_NAME, &name) == AI_SUCCESS && name.length > 0)
		snprintf(buf, size, "%s", name.data);
	else
		snprintf(buf, size, "mat_%u", mat_index);
}

static bool bind_aimp_texture(hz_material *m, void *buffer, size_t len)
{
	int w, h, n;
	uint8_t *data;
	bool ok;
	GLenum format;

	data = stbi_load_from_memory(buffer, len, &w, &h, &n, 0);
	if (!data) {
		return false;
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

	ok = hz_material_bind_texture(m, HZ_TEXTURE_DIFFUSE, data, format, w, h, true);

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

static bool import_textures(hz_material *m, struct aiMaterial *ai_mat,
		const struct aiScene *ai_scene)
{
	struct aiTexture *diffuse;

	diffuse = ai_get_texture(ai_mat, ai_scene, aiTextureType_DIFFUSE, 0);
	if (!diffuse) {
		return false;
	}

	return bind_aimp_texture(m, diffuse->pcData, diffuse->mWidth);
}

static void import_materials(hz_arena *arena, hz_material_store *store,
		const struct aiScene *ai_scene)
{
	for (unsigned i = 0; i < ai_scene->mNumMaterials; i++) {
		struct aiMaterial *ai_mat = ai_scene->mMaterials[i];
		char key[64];
		material_key(ai_scene, i, key, sizeof(key));

		hz_material *m = hz_material_new(arena);

		if (!import_textures(m, ai_mat, ai_scene))
			hz_log_e(tag, "unable to import textures for '%s'", key);

		hz_material_store_set(store, key, m);
		hz_log_i(tag, "imported material '%s'", key);
	}
}

static bool import_mesh_geometry(hz_node *n, struct aiMesh *ai_mesh)
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

	ok = hz_mesh_set_geometry(HZ_MESH(n),
		(hz_vec3 *)ai_mesh->mVertices, (hz_vec3 *)ai_mesh->mNormals,
		ai_mesh->mNumVertices,
		(hz_vec3 *)ai_mesh->mTextureCoords[0], ai_mesh->mNumVertices,
		indices, nindices);

	free(indices);

	return ok;
}

static hz_node *import_assimp_mesh(hz_arena *arena, hz_material_store *store,
		const struct aiScene *ai_scene, struct aiMesh *ai_mesh, hz_program *program)
{
	char key[64];
	material_key(ai_scene, ai_mesh->mMaterialIndex, key, sizeof(key));

	hz_material *material = hz_material_store_get(store, key);
	hz_node *n = hz_mesh_new(arena, program, material);

	if (!import_mesh_geometry(n, ai_mesh))
		return NULL;

	return n;
}

static hz_node *import_assimp_node(hz_arena *arena, const struct aiScene *scene,
		struct aiNode *node, hz_material_store *store, hz_program *program)
{
	hz_node *mesh, *root;
	struct aiMesh *ai_mesh;
	unsigned ai_imesh;

	root = hz_root_new(arena);

	hz_node_set_model(root, (void *)&node->mTransformation, true);

	for (size_t i = 0; i < node->mNumMeshes; i++) {
		ai_imesh = node->mMeshes[i];
		ai_mesh = scene->mMeshes[ai_imesh];

		hz_log_i(tag, "- found mesh '%s'", ai_mesh->mName.data);

		mesh = import_assimp_mesh(arena, store, scene, ai_mesh, program);
		if (!mesh) {
			hz_log_e(tag, "unable to import '%s'", ai_mesh->mName.data);
			continue;
		}

		hz_node_insert(root, mesh);
	}

	return root;
}

static hz_node *import_objects_recursive(hz_arena *arena, const struct aiScene *ai_scene,
		hz_node *parent, struct aiNode *ai_node,
		hz_material_store *store, hz_program *program, unsigned depth)
{
	struct aiNode *ai_child;
	hz_node *node;

	hz_log_i(tag, "(%d) looking at node '%s'", depth, ai_node->mName.data);

	node = import_assimp_node(arena, ai_scene, ai_node, store, program);
	if (parent && node) {
		hz_node_insert(parent, node);
	}

	for (size_t i = 0; i < ai_node->mNumChildren; i++) {
		ai_child = ai_node->mChildren[i];
		import_objects_recursive(arena, ai_scene, node, ai_child, store,
				program, depth + 1);
	}

	return node;
}

bool assimp_import_scene(hz_node *root, hz_arena *arena, hz_material_store *store,
		const char *path, hz_program *program)
{
	const struct aiScene *scene;
	unsigned int flags = aiProcess_FlipUVs | aiProcess_Triangulate
		| aiProcess_JoinIdenticalVertices;
	hz_node *imported = NULL;

	scene = aiImportFile(path, flags);
	if (!scene) {
		return false;
	}

	import_materials(arena, store, scene);

	imported = import_objects_recursive(arena, scene, NULL, scene->mRootNode,
			store, program, 0);

	aiReleaseImport(scene);
	hz_node_insert(root, imported);

	return true;
}
