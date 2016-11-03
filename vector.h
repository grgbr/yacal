#ifndef _VECTOR_H
#define _VECTOR_H

#include <stdlib.h>
#include "utils.h"

struct vector {
	unsigned int  vect_cnt;
	unsigned int  vect_sz;
	size_t        vect_ent_sz;
	char         *vect_arr;
};

#define VECT_INIT_SZ (4U)

#define vect_assert(_thiz)                            \
	ut_assert(_thiz);                             \
	ut_assert(_thiz->vect_cnt <= _thiz->vect_sz); \
	ut_assert(_thiz->vect_sz >= VECT_INIT_SZ);    \
	ut_assert(_thiz->vect_ent_sz > 0);            \
	ut_assert(_thiz->vect_arr);

static inline __nonull(1) __nothrow __pure
size_t vect_count(struct vector const* thiz)
{
	vect_assert(thiz);

	return thiz->vect_cnt;
}

static inline __nonull(1) __nothrow __pure
unsigned int vect_entry_size(struct vector const* thiz)
{
	vect_assert(thiz);

	return thiz->vect_ent_sz;
}

static inline __nonull(1) __nothrow __pure
char * vect_array(struct vector const* thiz)
{
	vect_assert(thiz);

	return thiz->vect_arr;
}

static inline __nonull(1) __nothrow
void vect_clear(struct vector* thiz)
{
	vect_assert(thiz);

	thiz->vect_cnt = 0;
}

static inline __nonull(1) __nothrow __pure
char * vect_entry(struct vector const* thiz, unsigned int index)
{
	vect_assert(thiz);
	ut_assert(index < thiz->vect_cnt);
	
	return thiz->vect_arr + (index * thiz->vect_ent_sz);
}

static inline __nonull(1) __nothrow
void vect_fini(struct vector const* thiz)
{
	vect_assert(thiz);

	free(thiz->vect_arr);
}

extern char* vect_push(struct vector*) __nonull(1);
extern void  vect_pop(struct vector*) __nonull(1);
extern void  vect_shrink(struct vector*) __nonull(1);
extern int   vect_init(struct vector*, size_t) __nonull(1);

#endif
