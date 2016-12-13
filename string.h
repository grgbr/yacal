#ifndef _YACAL_STRING_H
#define _YACAL_STRING_H

#include "vector.h"
#include "utils.h"

/******************************************************************************
 * C string utils
 ******************************************************************************/

extern size_t str_count_leading_blank(char const*, size_t)
              __nonull(1) __nothrow __leaf __pure;
extern size_t str_count_trailing_blank(char const*, size_t)
              __nonull(1) __nothrow __leaf __pure;
extern size_t str_count_leading_noblank(char const*, size_t)
              __nonull(1) __nothrow __leaf __pure;
extern size_t str_count_leading_alnum(char const*, size_t)
              __nonull(1) __nothrow __leaf __pure;
extern size_t str_count_trailing_alnum(char const*, size_t)
              __nonull(1) __nothrow __leaf __pure;
extern size_t str_count_leading_noalnum(char const*, size_t)
              __nonull(1) __nothrow __leaf __pure;
extern size_t str_count_blank_dups(char const*, size_t)
              __nonull(1) __nothrow __pure;
extern size_t str_eval_squeeze_blanks(char const*, size_t, size_t*)
              __nonull(1, 3) __nothrow __pure;
extern size_t str_squeeze_blanks(char* restrict, char const* restrict, size_t)
              __nonull(1, 2) __nothrow;

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

#endif
