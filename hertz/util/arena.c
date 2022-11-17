#include <hz/util/arena.h>
#include <hz/util/common.h>
#include <hz/adt/list.h>
#include <hz/util/logger.h>
#include <stdlib.h>

static const char *tag = "arena";

#define OBJECTS_PER_BUCKET	128

#define cast_bucket(ptr) \
	hz_container_of(ptr, hz_arena_bucket, head)

typedef struct _hz_arena_bucket hz_arena_bucket;
struct _hz_arena_bucket {
	hz_list_item head;
	const hz_arena_proto *proto;
	void *stack[OBJECTS_PER_BUCKET];
	unsigned ptr, alloc_size;
	bool full;
	void *mem[];
};

struct _hz_arena {
	hz_list_item bucket_list;
};

static inline unsigned align_size(unsigned size)
{
	const unsigned alignment = 4;
	return (size + alignment - 1) & ~(alignment - 1);
}

static void *bucket_alloc(hz_arena_bucket *bucket)
{
	void *mem = bucket->stack[bucket->ptr];
	bucket->ptr++;

	if (bucket->ptr >= OBJECTS_PER_BUCKET) {
		bucket->full = true;
	}

	return mem;
}

static hz_arena_bucket *create_bucket(hz_arena *arena, const hz_arena_proto *proto)
{
	hz_arena_bucket *bucket;
	unsigned alloc_size = align_size(proto->size);

	bucket = malloc(sizeof(hz_arena_bucket) + alloc_size * OBJECTS_PER_BUCKET);
	if (!bucket) {
		return NULL;
	}

	bucket->ptr = 0;
	bucket->proto = proto;
	bucket->full = false;
	hz_list_init(&bucket->head);

	for (unsigned i = 0; i < OBJECTS_PER_BUCKET; i++) {
		bucket->stack[i] = bucket->mem + alloc_size * i;
	}

	hz_list_push(&bucket->head, &arena->bucket_list);

	hz_log_d(tag, "created a bucket '%s' of alloc size %u",
			bucket->proto->name, alloc_size);

	return bucket;
}

static void free_bucket(hz_arena_bucket *bucket)
{
	hz_log_d(tag, "freeing bucket %s", bucket->proto->name);

	for (unsigned i = 0; i < bucket->ptr; i++) {
		if (bucket->proto->free) {
			bucket->proto->free(bucket->stack[i]);
		}
	}

	free(bucket);
}

static hz_arena_bucket *find_bucket(hz_arena *arena, const hz_arena_proto *proto)
{
	hz_list_item *item;
	hz_arena_bucket *bucket;

	hz_list_forward(item, &arena->bucket_list) {
		bucket = cast_bucket(item);

		if (!bucket->full && bucket->proto == proto) {
			return bucket;
		}
	}

	return NULL;
}

void *hz_arena_alloc(hz_arena *arena, const hz_arena_proto *proto)
{
	hz_arena_bucket *bucket;

	bucket  = find_bucket(arena, proto);
	if (!bucket) {
		bucket = create_bucket(arena, proto);
		if (!bucket) {
			return NULL;
		}
	}

	return bucket_alloc(bucket);
}

hz_arena *hz_arena_new(void)
{
	hz_arena *arena;

	arena = malloc(sizeof(hz_arena));
	if (!arena) {
		return NULL;
	}

	hz_list_init(&arena->bucket_list);

	return arena;
}

void hz_arena_free(hz_arena *arena)
{
	hz_list_item *item, *tmp;

	hz_list_forward_safe(item, tmp, &arena->bucket_list) {
		free_bucket(cast_bucket(item));
	}
}
