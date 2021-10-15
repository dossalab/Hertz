#include <stdlib.h>
#include <libgen.h>
#include <stdio.h>
#include <string.h>
#include <logger/logger.h>
#include <errors/errors.h>

#include <utils/files.h>
#include "obj_loader.h"

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

static char *prepare_mtlib_path(const char *obj_name, const char *mtl_file)
{
	char buffer[64];
	static char path[256];

	snprintf(buffer, sizeof(buffer), obj_name);
	dirname(buffer);

	snprintf(path, sizeof(path), "%s/%s", buffer, mtl_file);
	return path;
}

static int extract_texture_name_from_str(const char *obj_name,
		struct model *model, char *input)
{
	char *line;
	int ret = -1;

	while ((line = strsep(&input, "\n")) != NULL) {
		if (strncmp(line, "map_Kd", 6) == 0) {
			model->texture_path = strdup(
				prepare_mtlib_path(obj_name, line + 7));

			ret = 0;
			break;
		}
	}

	return ret;
}

static int process_material_lib(const char *obj_name, struct model *model,
		char *line)
{
	char *content;
	char *mtlib_path = prepare_mtlib_path(obj_name, line + 1);

	content = read_text_file(mtlib_path);
	if (!content) {
		log_i("[PS] no mtlib %s?", mtlib_path);

		/* we don't really consider it an error, yeah */
		return 0;
	}

	int ret = extract_texture_name_from_str(obj_name, model, content);
	free(content);

	return ret;
}

static void insert_vertex(struct model *m, struct vertex *v)
{
	m->vertices[m->vertex_count] = *v;
	m->vertex_count++;
}

static void insert_normal(struct model *m, struct vertex *v)
{
	m->normals[m->normal_count] = *v;
	m->normal_count++;
}

static void insert_uv(struct model *m, struct point *uv)
{
	/* uvs are stored inverted, (eg top is 0.0) */
	struct point uv_corrected;

	uv_corrected.x = uv->x;
	uv_corrected.y = 1.0 - uv->y;

	m->uvs[m->uv_count] = uv_corrected;
	m->uv_count++;
}

static void insert_new_vertex(struct vertex *v, struct obj_parser *parser)
{
	const size_t chunksize = 128;
	size_t space = parser->vertice_allocated_size - sizeof(struct vertex) * parser->vertice_count;
	void *tmp;

	if (space < sizeof(struct vertex)) {
		tmp = realloc(parser->vertices, parser->vertice_allocated_size + chunksize);
		if (!tmp) {
			log_e("no mem left!");
			return;
		}

		parser->vertice_allocated_size += chunksize;
		parser->vertices = tmp;
	}

	parser->vertices[parser->vertice_count] = *v;
	parser->vertice_count++;
}

static void insert_new_normal(struct vertex *n, struct obj_parser *parser)
{
	const size_t chunksize = 128;
	size_t space = parser->normal_allocated_size - sizeof(struct vertex) * parser->normal_count;
	void *tmp;

	if (space < sizeof(struct vertex)) {
		tmp = realloc(parser->normals, parser->normal_allocated_size + chunksize);
		if (!tmp) {
			log_e("no mem left!");
			return;
		}

		parser->normal_allocated_size += chunksize;
		parser->normals = tmp;
	}

	parser->normals[parser->normal_count] = *n;
	parser->normal_count++;
}

static void insert_new_uv(struct point *uv, struct obj_parser *parser)
{
	const size_t chunksize = 128;
	void *tmp;
	size_t space = parser->uv_allocated_size - sizeof(struct point) * parser->uv_count;

	if (space < sizeof(struct point)) {
		tmp = realloc(parser->uvs, parser->uv_allocated_size + chunksize);
		if (!tmp) {
			log_e("no mem left!");
			return;
		}

		parser->uv_allocated_size += chunksize;
		parser->uvs = tmp;
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
		struct obj_parser *parser, struct model *model)
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
			log_e("[PS] unable to extract f at %d, matches were %d",
					line_counter, matches);
			return -1;
		}
	}

	/* indexes are count from 1, not from 0 */
	if (vertex_id[0] > parser->vertice_count ||
			vertex_id[1] > parser->vertice_count ||
			vertex_id[2] > parser->vertice_count) {
		log_e("[PS] file seems malformed, index is not found");
		return -1;
	}

	/* normals are count from 1, not from 0 */
	if (normal_id[0] > parser->normal_count ||
			normal_id[1] > parser->normal_count ||
			normal_id[2] > parser->normal_count) {
		log_e("[PS] file seems malformed, index is not found");
		return -1;
	}

	v1 = parser->vertices[vertex_id[0] - 1];
	v2 = parser->vertices[vertex_id[1] - 1];
	v3 = parser->vertices[vertex_id[2] - 1];

	n1 = parser->normals[normal_id[0] - 1];
	n2 = parser->normals[normal_id[1] - 1];
	n3 = parser->normals[normal_id[2] - 1];

	size_t vspace = parser->mesh_allocated_vertices - model->vertex_count;
	if (vspace < sizeof(struct vertex) * 3) {
		parser->mesh_allocated_vertices += 128;
		model->vertices = realloc(model->vertices, sizeof(struct vertex) * \
				parser->mesh_allocated_vertices);
	}

	size_t nspace = parser->mesh_allocated_normals - model->normal_count;
	if (nspace < sizeof(struct vertex) * 3) {
		parser->mesh_allocated_normals += 128;
		model->normals = realloc(model->normals, sizeof(struct vertex) * \
				parser->mesh_allocated_normals);
	}

	insert_normal(model, &n1);
	insert_normal(model, &n2);
	insert_normal(model, &n3);

	insert_vertex(model, &v1);
	insert_vertex(model, &v2);
	insert_vertex(model, &v3);

	/* with uv */
	if (matches == 9) {
		struct point uv1, uv2, uv3;

		/* uvs are count from 1, not from 0 */
		if (uv_id[0] > parser->uv_count ||
				uv_id[1] > parser->uv_count ||
				uv_id[2] > parser->uv_count) {
			log_e("[PS] file seems malformed, index is not found");
			return -1;
		}

		uv1 = parser->uvs[uv_id[0] - 1];
		uv2 = parser->uvs[uv_id[1] - 1];
		uv3 = parser->uvs[uv_id[2] - 1];

		size_t uspace = parser->mesh_allocated_uvs - model->uv_count;
		if (uspace < sizeof(struct point) * 3) {
			parser->mesh_allocated_uvs += 128;
			model->uvs = realloc(model->uvs, sizeof(struct vertex) * \
					parser->mesh_allocated_uvs);
		}

		insert_uv(model, &uv1);
		insert_uv(model, &uv2);
		insert_uv(model, &uv3);
	}

	return 0;
}

static int process_obj_line(size_t line_counter, struct obj_parser *parser,
		struct model *model, char *line, const char *filename)
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
		log_e("[PS] got g");
		break;

	case 'f':
		return try_extract_face(line_counter, line + 1, parser, model);

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
			process_material_lib(filename, model, line + 6);
			return 0;
		}
		
		return -1;

	case 'u':
		if (strncmp(line, "usemtl", 6) == 0) {
			return 0;
		}
		
		return -1;

	case 'l':
		/* don't know what this is but ok */
		return 0;

	default:
		log_e("[PS] malformed char at %d: '%c' (0x%02X)",
				line_counter, line[0], line[0]);
		return -1;
	}

	/* should not appear here */
	return -1;
}

static int extract_obj_from_str(struct model *model, char *input,
		const char *filename)
{
	char *line;
	size_t line_counter = 1;
	int ret = 0;
	struct obj_parser parser;

	memset(&parser, 0, sizeof(struct obj_parser));
	memset(model, 0, sizeof(struct model));

	while ((line = strsep(&input, "\n")) != NULL) {
		ret = process_obj_line(line_counter, &parser, model, line, filename);
		if (ret < 0) {
			break;
		}

		line_counter++;
	}

	return ret;
}

int loader_load_obj(struct model *model, const char *filename)
{
	char *content;
	int ret;

	content = read_text_file(filename);
	if (!content) {
		return -ERR_NO_FILE;
	}

	ret = extract_obj_from_str(model, content, filename);
	free(content);

	return ret;
}
