#include <errno.h>
#include <string.h>
#include "utils.h"
#include "dstr.h"

static __nonull(1) __nothrow
int dstr_resize(struct dstr* thiz, size_t size)
{
	ut_assert(size >= thiz->dstr_min);

	size_t const sz = 1U << ut_upper_pow2(size);
	char*        ptr;

	ptr = realloc(thiz->dstr_ptr, sz);
	if (!ptr)
		return -ENOMEM;

	thiz->dstr_ptr = ptr;
	thiz->dstr_sz = sz;

	return 0;
}

static __nonull(1) __nothrow
int dstr_grow(struct dstr* thiz, size_t size)
{
	if (size > thiz->dstr_sz)
		return dstr_resize(thiz, size);

	return 0;
}

__nonull(1, 2) __nothrow
int dstr_cpy(struct dstr* thiz, char const* string)
{
	dstr_assert(thiz);
	ut_assert(string);

	size_t len;
	int    err;

	len = strlen(string);

	err = dstr_grow(thiz, len + 1);
	if (err)
		return err;

	memcpy(&thiz->dstr_ptr[0], string, len + 1);

	thiz->dstr_len = len;

	return 0;
}

__nonull(1, 2) __nothrow
int dstr_cat(struct dstr* thiz, char const* string)
{
	dstr_assert(thiz);
	ut_assert(string);

	size_t len;
	int    err;

	len = strlen(string);
	if (!len)
		return 0;

	err = dstr_grow(thiz, thiz->dstr_len + len + 1);
	if (err)
		return err;

	memcpy(&thiz->dstr_ptr[thiz->dstr_len], string, len + 1);

	thiz->dstr_len += len;

	return 0;
}

__nonull(1) __nothrow
int dstr_shrink(struct dstr* thiz)
{
	dstr_assert(thiz);

	size_t const sz = 1U << ut_upper_pow2(thiz->dstr_len + 1);

	if ((sz > thiz->dstr_min) && (thiz->dstr_sz > sz))
		return dstr_resize(thiz, sz);

	return 0;
}

__nonull(1) __nothrow __leaf
int dstr_init(struct dstr* thiz, size_t size)
{
	ut_assert(thiz);
	ut_assert(size);

	size_t const sz = 1U << ut_upper_pow2(size);

	thiz->dstr_ptr = malloc(sz);
	if (!thiz->dstr_ptr)
		return -ENOMEM;

	thiz->dstr_len = 0;
	thiz->dstr_sz = sz;
	thiz->dstr_min = sz;

	return 0;
}
