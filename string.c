#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "string.h"

/******************************************************************************
 * C string utils
 ******************************************************************************/

__nonull(1) __nothrow __leaf __pure
size_t str_count_leading_blank(char const* string, size_t length)
{
	ut_assert(string);

	size_t pos;

	for (pos = 0; pos < length; pos++) {
		ut_assert(string[pos] != '\0');

		if (!isblank(string[pos]))
			break;
	}

	return pos;
}

__nonull(1) __nothrow __leaf __pure
size_t str_count_trailing_blank(char const* string, size_t length)
{
	ut_assert(string);

	size_t pos;

	for (pos = length; pos > 0; pos--) {
		ut_assert(string[pos - 1] != '\0');

		if (!isblank(string[pos - 1]))
			break;
	}

	return length - pos;
}

__nonull(1) __nothrow __leaf __pure
size_t str_count_leading_noblank(char const* string, size_t length)
{
	ut_assert(string);

	size_t pos;

	for (pos = 0; pos < length; pos++) {
		ut_assert(string[pos] != '\0');

		if (isblank(string[pos]))
			break;
	}

	return pos;
}

__nonull(1) __nothrow __leaf __pure
size_t str_count_leading_alnum(char const* string, size_t length)
{
	ut_assert(string);

	size_t pos;

	for (pos = 0; pos < length; pos++) {
		ut_assert(string[pos] != '\0');

		if (!isalnum(string[pos]))
			break;
	}

	return pos;
}

__nonull(1) __nothrow __leaf __pure
size_t str_count_leading_noalnum(char const* string, size_t length)
{
	ut_assert(string);

	size_t pos;

	for (pos = 0; pos < length; pos++) {
		ut_assert(string[pos] != '\0');

		if (isalnum(string[pos]))
			break;
	}

	return pos;
}

__nonull(1) __nothrow __leaf __pure
size_t str_count_trailing_alnum(char const* string, size_t length)
{
	ut_assert(string);

	size_t pos;

	for (pos = length; pos > 0; pos--) {
		ut_assert(string[pos - 1] != '\0');

		if (!isalnum(string[pos - 1]))
			break;
	}

	return length - pos;
}

__nonull(1) __nothrow __pure
size_t str_count_blank_dups(char const* string, size_t length)
{
	ut_assert(string && (*string != '\0'));
	ut_assert(length);

	size_t dups = 0;

	do {
		size_t len;

		len = str_count_leading_blank(string, length);
		if (len > 1)
			dups += len - 1;

		string += len;
		length -= len;

		len = str_count_leading_noblank(string, length);

		string += len;
		length -= len;
	} while (length);

	return dups;
}

__nonull(1, 3) __nothrow __pure
size_t str_eval_squeeze_blanks(char const* string,
                               size_t      length,
                               size_t*     available)
{
	ut_assert(string && (*string != '\0'));
	ut_assert(length);
	ut_assert(available && *available);

	char const* const start = string;

	do {
		size_t len;

		len = str_count_leading_blank(string, length);
		if (len >= 1)
			*available -= 1;

		string += len;

		if (!*available)
			break;

		length -= len;

		len = str_count_leading_noblank(string, length);

		if (len >= *available) {
			string += *available;
			*available = 0;
			break;
		}

		*available -= len;

		string += len;
		length -= len;
	} while (length);

	return string - start;
}

__nonull(1, 2) __nothrow
size_t str_squeeze_blanks(char* restrict       dst,
                          char const* restrict src,
                          size_t               length)
{
	ut_assert(dst);
	ut_assert(src && (*src != '\0'));
	ut_assert(length);

	char const* const start = dst;
	size_t            cnt = 0;

	do {
		size_t blanks;
		size_t len;

		blanks = str_count_leading_blank(src + cnt, length);
		len = blanks + str_count_leading_noblank(src + cnt,
		                                         length - blanks);

		if (blanks > 1) {
			memcpy(dst, src, cnt);
			src += cnt + blanks - 1;
			dst += cnt;
			cnt = 1;
		}
		else
			cnt += len;

		length -= len;
	} while (length);

	memcpy(dst, src, cnt);

	return dst + cnt - start;
}

char* str_strip_blank(char* string)
{
	size_t len;
	size_t blanks;

	len = strlen(string);

	blanks = str_count_leading_blank(string, len);
	string += blanks;
	len -= blanks;

	len -= str_count_trailing_blank(string, len);
	string[len] = '\0';

	return string;
}

/******************************************************************************
 * Dynamically resizeable string
 ******************************************************************************/

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
