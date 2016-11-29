#ifndef _YACAL_STRING_H
#define _YACAL_STRING_H

#include "vector.h"
#include "utils.h"

/******************************************************************************
 * Dynamically resizeable string
 ******************************************************************************/

struct dstr {
	size_t dstr_len;
	size_t dstr_sz;
	char*  dstr_ptr;
	size_t dstr_min;
};

#define dstr_assert(_dstr)                           \
	ut_assert(_dstr);                            \
	ut_assert(_dstr->dstr_len < _dstr->dstr_sz); \
	ut_assert(_dstr->dstr_ptr);                  \
	ut_assert(_dstr->dstr_sz >= _dstr->dstr_min)

static inline __nonull(1) __nothrow __pure
size_t dstr_len(struct dstr const* thiz)
{
	dstr_assert(thiz);

	return thiz->dstr_len;
}

static inline __nonull(1) __nothrow __pure
char const* dstr_charp(struct dstr const* thiz)
{
	dstr_assert(thiz);

	return thiz->dstr_ptr;
}

static inline __nonull(1) __nothrow
void dstr_trunc(struct dstr* thiz, size_t len)
{
	dstr_assert(thiz);
	ut_assert(len <= thiz->dstr_len);

	thiz->dstr_len = len;
	thiz->dstr_ptr[len] = '\0';
}

static inline __nonull(1) __nothrow
void dstr_fini(struct dstr const* thiz)
{
	dstr_assert(thiz);

	free(thiz->dstr_ptr);
}

extern int dstr_cpy(struct dstr*, char const*) __nonull(1, 2) __nothrow;
extern int dstr_cat(struct dstr*, char const*) __nonull(1, 2) __nothrow;
extern int dstr_shrink(struct dstr*) __nonull(1) __nothrow;
extern int dstr_init(struct dstr*, size_t) __nonull(1) __nothrow __leaf;

/******************************************************************************
 * Strings line splitter
 ******************************************************************************/

struct line_bundle {
	struct vector lbndl_store;
	unsigned int  lbndl_lncnt;
};

#define lbndl_assert(_bndl) \
	ut_assert(_bndl);

static inline __nonull(1) __nothrow __pure
unsigned int lbndl_line_count(struct line_bundle const* thiz)
{
	lbndl_assert(thiz);

	return thiz->lbndl_lncnt + !!vect_count(&thiz->lbndl_store);
}

static inline __nonull(1) __nothrow __pure
unsigned int lbndl_atom_count(struct line_bundle const* thiz)
{
	lbndl_assert(thiz);

	return vect_count(&thiz->lbndl_store);
}

static inline __nonull(1) __nothrow
void lbndl_fini(struct line_bundle const* thiz)
{
	lbndl_assert(thiz);

	return vect_fini(&thiz->lbndl_store);
}

extern char const* lbndl_atom(struct line_bundle const*,
                              unsigned int,
                              size_t*)
                   __nonull(1, 3) __nothrow __leaf;

extern int         lbndl_push_newline(struct line_bundle*)
                   __nonull(1) __nothrow __leaf;

extern int         lbndl_push_string(struct line_bundle*, char const*, size_t)
                   __nonull(1, 2) __nothrow;

extern void        lbndl_clear(struct line_bundle*)
                   __nonull(1) __nothrow __leaf;

extern int         lbndl_init(struct line_bundle*)
                   __nonull(1) __nothrow __leaf;

#endif
