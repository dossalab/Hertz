#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <hz/logger.h>
#include <stdlib.h>
#include <assert.h>
#include <hz/types.h>
#include <hz/object.h>
#include <hz/objects/mesh.h>
#include <hz/objects/root.h>
#include <hz/material.h>

#include "vendor/stb/stb_image.h"

static const char *tag = "aimp";

/* Make sure we can copy assimp buffers directly */
static_assert(sizeof(ai_real) == sizeof(float), "ai_real is not float");

extern int assimp_shader;

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

static bool import_textures(struct hz_material *m, struct aiMaterial *ai_mat,
		const struct aiScene *ai_scene)
{
	struct aiTexture *diffuse;

	diffuse = ai_get_texture(ai_mat, ai_scene, aiTextureType_DIFFUSE, 0);
	if (!diffuse) {
		return false;
	}

	return bind_aimp_texture(m, diffuse->pcData, diffuse->mWidth);
}

static struct hz_material *import_assimp_material(struct aiMaterial *ai_mat,
		const struct aiScene *ai_scene)
{
	struct hz_material *m;
	bool ok;

	m = hz_material_new(assimp_shader);
	if (!m) {
		return NULL;
	}

	ok = import_textures(m, ai_mat, ai_scene);
	if (!ok) {
		/* TODO: get material name */
		hz_log_e(tag, "unable to import textures for '%s'", "x");

		/* not an error? */
	}

	return m;
}

static struct hz_material **import_materials(const struct aiScene *ai_scene)
{
	struct aiMaterial *ai_mat;
	struct hz_material *m;
	struct hz_material **materials;

	materials = calloc(1, sizeof(struct hz_material *) * ai_scene->mNumMaterials);
	if (!materials) {
		return NULL;
	}

	for (unsigned i = 0; i < ai_scene->mNumMaterials; i++) {
		ai_mat = ai_scene->mMaterials[i];

		m = import_assimp_material(ai_mat, ai_scene);
		if (!m) {
			/* TODO: get material name */
			hz_log_e(tag, "unable to import material '%s'", "x");
			continue;
		}

		materials[i] = m;
	}

	return materials;
}

static bool import_mesh_geometry(struct hz_object *o, struct aiMesh *ai_mesh)
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

	ok = hz_mesh_set_geometry(HZ_MESH(o),
		(hz_vec3 *)ai_mesh->mVertices, (hz_vec3 *)ai_mesh->mNormals,
		ai_mesh->mNumVertices,
		(hz_vec3 *)ai_mesh->mTextureCoords[0], ai_mesh->mNumVertices,
		indices, nindices);

	free(indices);

	return ok;
}

static struct hz_object *import_assimp_mesh(struct aiMesh *ai_mesh,
		struct hz_material **materials)
{
	struct hz_object *o;
	bool ok;

	o = hz_mesh_new(assimp_shader, materials[ai_mesh->mMaterialIndex]);
	if (!o) {
		return false;
	}

	ok = import_mesh_geometry(o, ai_mesh);
	if (!ok) {
		/* Free mesh */
		return NULL;
	}

	return o;
}

static struct hz_object *import_assimp_node(const struct aiScene *scene,
		struct aiNode *node, struct hz_material **materials)
{
	struct hz_object *mesh, *root;
	struct aiMesh *ai_mesh;
	unsigned ai_imesh;

	root = hz_root_new();
	if (!root) {
		return NULL;
	}

	hz_object_set_model(root, (void *)&node->mTransformation, true);

	for (size_t i = 0; i < node->mNumMeshes; i++) {
		ai_imesh = node->mMeshes[i];
		ai_mesh = scene->mMeshes[ai_imesh];

		hz_log_i(tag, "- found mesh '%s'", ai_mesh->mName.data);

		mesh = import_assimp_mesh(ai_mesh, materials);
		if (!mesh) {
			hz_log_e(tag, "unable to import '%s'", ai_mesh->mName.data);
			continue;
		}

		hz_object_insert(root, mesh);
	}

	return root;
}

static struct hz_object *import_objects_recursive(const struct aiScene *ai_scene,
		struct hz_object *parent, struct aiNode *ai_node,
		struct hz_material **materials, unsigned depth)
{
	struct aiNode *ai_child;
	struct hz_object *object;

	hz_log_i(tag, "(%d) looking at node '%s'", depth, ai_node->mName.data);

	object = import_assimp_node(ai_scene, ai_node, materials);
	if (parent && object) {
		hz_object_insert(parent, object);
	}

	for (size_t i = 0; i < ai_node->mNumChildren; i++) {
		ai_child = ai_node->mChildren[i];
		import_objects_recursive(ai_scene, object, ai_child, materials,
				depth + 1);
	}

	return object;
}

struct hz_object *assimp_import_scene(const char *path)
{
	const struct aiScene *scene;
	unsigned int flags = aiProcess_FlipUVs | aiProcess_Triangulate
		| aiProcess_JoinIdenticalVertices;
	struct hz_object *root = NULL;
	struct hz_material **materials;

	scene = aiImportFile(path, flags);
	if (!scene) {
		return false;
	}

	materials = import_materials(scene);
	if (!materials) {
		goto cleanup;
	}

	root = import_objects_recursive(scene, NULL, scene->mRootNode, materials, 0);

cleanup:
	aiReleaseImport(scene);
	return root;
}
