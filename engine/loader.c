#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <libgen.h>
#include <stdio.h>
#include <string.h>
#include <engine/util.h>
#include <engine/loader.h>

struct obj_parser {
	struct vertex *vertices;
	size_t vertice_count;
	size_t vertice_allocated_size;

	struct vertex *normals;
	size_t normal_count;
	size_t normal_allocated_size;

	struct point *uvs;
	size_t uv_count;
	size_t uv_allocated_size;

	size_t mesh_allocated_vertices;
	size_t mesh_allocated_normals;
	size_t mesh_allocated_uvs;
};

static char *terminate_region(struct region *r)
{
	if (r->size >= r->alloc) {
		r->ptr = realloc(r->ptr, r->size + 8);
	}

	((char *)r->ptr)[r->size] = '\0';
	return (char *)r->ptr;
}

static char *prepare_mtlib_path(const char *obj_name, const char *mtl_file)
{
	char buffer[64];
	static char path[256];

	snprintf(buffer, sizeof(buffer), obj_name);
	dirname(buffer);

	snprintf(path, sizeof(path), "%s/%s", buffer, mtl_file);
	return path;
}

static int extract_texture_name_from_chunk(const char *obj_name,
		struct mesh *mesh, struct region *data)
{
	char *line, *input;
	int ret = -1;

	input = terminate_region(data);

	while ((line = strsep(&input, "\n")) != NULL) {
		if (strncmp(line, "map_Kd", 6) == 0) {
			mesh->texture_path = strdup(
				prepare_mtlib_path(obj_name, line + 7));

			ret = 0;
			break;
		}
	}

	return ret;
}

static int process_material_lib(const char *obj_name, struct mesh *mesh,
		char *line)
{
	struct region data;
	char *mtlib_path = prepare_mtlib_path(obj_name, line + 1);

	if (read_path(mtlib_path,  &data)) {
		logger_report("[PS] no mtlib %s?\n", mtlib_path);

		/* we don't really consider it an error, yeah */
		return 0;
	}

	int ret = extract_texture_name_from_chunk(obj_name, mesh, &data);
	free(data.ptr);

	return ret;
}

static void insert_vertex(struct mesh *m, struct vertex *v)
{
	m->vertices[m->vertice_cnt] = *v;
	m->vertice_cnt++;
}

static void insert_normal(struct mesh *m, struct vertex *v)
{
	m->normals[m->normal_cnt] = *v;
	m->normal_cnt++;
}

static void insert_uv(struct mesh *m, struct point *uv)
{
	/* uvs are stored inverted, (eg top is 0.0) */
	struct point uv_corrected;

	uv_corrected.x = uv->x;
	uv_corrected.y = 1.0 - uv->y;

	m->tex_coords[m->tex_coord_cnt] = uv_corrected;
	m->tex_coord_cnt++;
}

static void insert_new_vertex(struct vertex *v, struct obj_parser *parser)
{
	const size_t chunksize = 128;
	size_t space = parser->vertice_allocated_size - sizeof(struct vertex) * parser->vertice_count;

	if (space < sizeof(struct vertex)) {
		parser->vertice_allocated_size += chunksize;
		parser->vertices = realloc(parser->vertices, parser->vertice_allocated_size);
	}

	parser->vertices[parser->vertice_count] = *v;
	parser->vertice_count++;
}

static void insert_new_normal(struct vertex *n, struct obj_parser *parser)
{
	const size_t chunksize = 128;
	size_t space = parser->normal_allocated_size - sizeof(struct vertex) * parser->normal_count;

	if (space < sizeof(struct vertex)) {
		parser->normal_allocated_size += chunksize;
		parser->normals = realloc(parser->normals, parser->normal_allocated_size);
	}

	parser->normals[parser->normal_count] = *n;
	parser->normal_count++;
}

static void insert_new_uv(struct point *uv, struct obj_parser *parser)
{
	const size_t chunksize = 128;
	size_t space = parser->uv_allocated_size - sizeof(struct point) * parser->uv_count;

	if (space < sizeof(struct point)) {
		parser->uv_allocated_size += chunksize;
		parser->uvs = realloc(parser->uvs , parser->uv_allocated_size);
	}

	parser->uvs[parser->uv_count] = *uv;
	parser->uv_count++;
}

static int extract_v_command(struct vertex *result, char *line)
{
	int ret = sscanf(line, "%f %f %f", &result->x, &result->y, &result->z);
	if (ret != 3) {
		return -1;
	} else {
		return 0;
	}
}

static int extract_vt_command(struct point *result, char *line)
{
	int ret = sscanf(line, "%f %f", &result->x, &result->y);
	if (ret != 2) {
		return -1;
	} else {
		return 0;
	}
}

static int try_extract_face(size_t line_counter, const char *line,
		struct obj_parser *parser, struct mesh *mesh)
{
	unsigned int vertex_id[3], uv_id[3], normal_id[3];
	struct vertex v1, v2, v3, n1, n2, n3;

	int matches = sscanf(line, "%d/%d/%d %d/%d/%d %d/%d/%d",
				&vertex_id[0],
				&uv_id[0],
				&normal_id[0],
				&vertex_id[1],
				&uv_id[1],
				&normal_id[1],
				&vertex_id[2],
				&uv_id[2],
				&normal_id[2]);
	if (matches != 9) {
		/* try without uv */
		matches = sscanf(line, "%d//%d %d//%d %d//%d",
					&vertex_id[0],
					&normal_id[0],
					&vertex_id[1],
					&normal_id[1],
					&vertex_id[2],
					&normal_id[2]);
		if (matches != 6) {
			logger_report("[PS] unable to extract f at %d, matches were %d\n",
					line_counter, matches);
			return -1;
		}
	}

	/* indexes are count from 1, not from 0 */
	if (vertex_id[0] > parser->vertice_count ||
			vertex_id[1] > parser->vertice_count ||
			vertex_id[2] > parser->vertice_count) {
		logger_report("[PS] file seems malformed, index is not found\n");
		return -1;
	}

	/* normals are count from 1, not from 0 */
	if (normal_id[0] > parser->normal_count ||
			normal_id[1] > parser->normal_count ||
			normal_id[2] > parser->normal_count) {
		logger_report("[PS] file seems malformed, index is not found\n");
		return -1;
	}

	v1 = parser->vertices[vertex_id[0] - 1];
	v2 = parser->vertices[vertex_id[1] - 1];
	v3 = parser->vertices[vertex_id[2] - 1];

	n1 = parser->normals[normal_id[0] - 1];
	n2 = parser->normals[normal_id[1] - 1];
	n3 = parser->normals[normal_id[2] - 1];

	size_t vspace = parser->mesh_allocated_vertices - mesh->vertice_cnt;
	if (vspace < sizeof(struct vertex) * 3) {
		parser->mesh_allocated_vertices += 128;
		mesh->vertices = realloc(mesh->vertices, sizeof(struct vertex) * \
				parser->mesh_allocated_vertices);
	}

	size_t nspace = parser->mesh_allocated_normals - mesh->normal_cnt;
	if (nspace < sizeof(struct vertex) * 3) {
		parser->mesh_allocated_normals += 128;
		mesh->normals = realloc(mesh->normals, sizeof(struct vertex) * \
				parser->mesh_allocated_normals);
	}

	insert_normal(mesh, &n1);
	insert_normal(mesh, &n2);
	insert_normal(mesh, &n3);

	insert_vertex(mesh, &v1);
	insert_vertex(mesh, &v2);
	insert_vertex(mesh, &v3);

	/* with uv */
	if (matches == 9) {
		struct point uv1, uv2, uv3;

		/* uvs are count from 1, not from 0 */
		if (uv_id[0] > parser->uv_count ||
				uv_id[1] > parser->uv_count ||
				uv_id[2] > parser->uv_count) {
			logger_report("[PS] file seems malformed, index is not found\n");
			return -1;
		}

		uv1 = parser->uvs[uv_id[0] - 1];
		uv2 = parser->uvs[uv_id[1] - 1];
		uv3 = parser->uvs[uv_id[2] - 1];

		size_t uspace = parser->mesh_allocated_uvs - mesh->tex_coord_cnt;
		if (uspace < sizeof(struct point) * 3) {
			parser->mesh_allocated_uvs += 128;
			mesh->tex_coords= realloc(mesh->tex_coords, sizeof(struct vertex) * \
					parser->mesh_allocated_uvs);
		}

		insert_uv(mesh, &uv1);
		insert_uv(mesh, &uv2);
		insert_uv(mesh, &uv3);
	}

	return 0;
}

static int process_obj_line(size_t line_counter, struct obj_parser *parser,
		struct mesh *mesh, char *line, const char *filename)
{
	switch (line[0]) {
	case 'o':
	case 's':
	case '#':
	case '\n':
	case '\r':
	case '\0': /* empty line (\n) */
		return 0;

	case 'g':
		logger_report("[PS] got g\n");
		break;

	case 'f':
		return try_extract_face(line_counter, line + 1, parser, mesh);

	case 'v':
		if (line[1] == 't') {
			/* vt */
			struct point v;

			if (extract_vt_command(&v, line + 2) < 0) {
				return -1;
			}

			insert_new_uv(&v, parser);
			return 0;

		} else if (line[1] == 'n') {
			/* vn */
			struct vertex v;

			if (extract_v_command(&v, line + 2) < 0) {
				return -1;
			}

			insert_new_normal(&v, parser);
			return 0;

		} else if (line[1] == ' ') {
			/* v */
			struct vertex v;

			if (extract_v_command(&v, line + 2) < 0) {
				return -1;
			}

			insert_new_vertex(&v, parser);
			return 0;

		} else {
			 return -1;
		}

		/* should not reach */
		return -1;
	
	case 'm':
		if (strncmp(line, "mtllib", 6) == 0) {
			process_material_lib(filename, mesh, line + 6);
			return 0;
		}
		
		return -1;

	case 'u':
		if (strncmp(line, "usemtl", 6) == 0) {
			return 0;
		}
		
		return -1;

	default:
		logger_report("[PS] malformed char at %d: '%c' (0x%02X)\n",
				line_counter, line[0], line[0]);
		return -1;
	}

	/* should not appear here */
	return -1;
}

static int extract_obj_from_chunk(struct mesh *mesh, struct region *data,
		const char *filename)
{
	char *line, *input;
	size_t line_counter = 1;
	int ret = 0;

	struct obj_parser parser = { };
	memset(mesh, 0, sizeof(struct mesh));

	input = terminate_region(data);

	while ((line = strsep(&input, "\n")) != NULL) {
		ret = process_obj_line(line_counter, &parser, mesh, line, filename);
		if (ret < 0) {
			break;
		}

		line_counter++;
	}

	return ret;
}

int loader_load_obj(struct mesh *mesh, const char *filename)
{
	struct region data;
	int ret;

	if (read_path(filename,  &data)) {
		logger_report("[PS] Unable to load file %s\n", filename);
		return -1;
	}

	ret = extract_obj_from_chunk(mesh, &data, filename);
	free(data.ptr);

	return ret;
}

int loader_load(struct mesh *mesh, const char *filename)
{
	struct region data;
	void *ptr;
	int entries, size;

	if (read_path(filename,  &data)) {
		logger_report("[PS] Unable to load file %s\n", filename);
		return -1;
	}

	if (data.size <= 84) {
		logger_report("[PS] File %s is too small\n", filename);
		free(data.ptr);
		return -1;
	}

	entries = *((unsigned int *)(data.ptr + 80));
	size = 84 + entries * (sizeof(struct stl_entry) + 2);

	if (size > data.size) {
		logger_report("[PS] Seems like %s is corrupt!\n", filename);
		free(data.ptr);
		return -1;
	}

	ptr = data.ptr + 84; /* skip header + count*/
	logger_report("[PS] %d vertices\n", entries);

	mesh->vertices = malloc(sizeof(struct vertex) * size * 3);
	mesh->normals = malloc(sizeof(struct vertex) * size * 3);
	mesh->vertice_cnt = 0;
	mesh->normal_cnt = 0;
	mesh->tex_coord_cnt = 0;

	for (int i = 0; i < entries; i++) {
		struct stl_entry *entry = ptr;

		insert_vertex(mesh, &entry->v1);
		insert_vertex(mesh, &entry->v2);
		insert_vertex(mesh, &entry->v3);

		insert_normal(mesh, &entry->normal);
		insert_normal(mesh, &entry->normal);
		insert_normal(mesh, &entry->normal);

		ptr += sizeof(struct stl_entry);
		ptr += 2;
	}

	free(data.ptr);
	return 0;
}

