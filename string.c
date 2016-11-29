#include <string.h>
#include <errno.h>
#include "string.h"

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

/******************************************************************************
 * Strings line splitter
 ******************************************************************************/

struct line_bundle_atom {
	char const* lbndl_str;
	size_t      lbndl_len;
};

__nonull(1, 3) __nothrow __leaf
char const* lbndl_atom(struct line_bundle const* thiz,
                       unsigned int              index,
                       size_t*                   length)
{
	lbndl_assert(thiz);

	struct line_bundle_atom* atm;

	atm = (struct line_bundle_atom*)vect_entry(&thiz->lbndl_store, index);
	if (atm->lbndl_str)
		*length = atm->lbndl_len;

	return atm->lbndl_str;
}

__nonull(1) __nothrow __leaf
int lbndl_push_newline(struct line_bundle* thiz)
{
	lbndl_assert(thiz);

	struct line_bundle_atom* atm;

	atm = (struct line_bundle_atom*)vect_push(&thiz->lbndl_store);
	if (!atm)
		return -errno;

	atm->lbndl_str = NULL;
	thiz->lbndl_lncnt++;

	return 0;
}

__nonull(1, 2) __nothrow
int lbndl_push_string(struct line_bundle* thiz,
                      char const*         string,
                      size_t              atom_size)
{
	lbndl_assert(thiz);
	ut_assert(string && (string[0] != '\0'));
	ut_assert(atom_size);

	do {
		struct line_bundle_atom* atm;
		size_t                   len;

		atm = (struct line_bundle_atom*)vect_push(&thiz->lbndl_store);
		if (!atm)
			return -errno;

		if (*string == '\n') {
			atm->lbndl_str = NULL;
			thiz->lbndl_lncnt++;

			string += 1 + strspn(string + 1, " ");
			continue;
		}

		atm->lbndl_str = string;

		len = strcspn(string, "\n");
		if (len < atom_size) {
			atm->lbndl_len = len;

			string += len;
			continue;
		}

		atm->lbndl_len = atom_size;

		if (len > atom_size) {
			int err;

			err = lbndl_push_newline(thiz);
			if (err)
				return err;

			string += atom_size + strspn(string + atom_size, " ");
			continue;
		}

		string += atom_size;
	} while (*string != '\0');

	return 0;
}

__nonull(1) __nothrow __leaf
void lbndl_clear(struct line_bundle* thiz)
{
	lbndl_assert(thiz);

	vect_clear(&thiz->lbndl_store);
	thiz->lbndl_lncnt = 0;
}

__nonull(1) __nothrow __leaf
int lbndl_init(struct line_bundle* thiz)
{
	ut_assert(thiz);

	thiz->lbndl_lncnt = 0;

	return vect_init(&thiz->lbndl_store,
	                 sizeof(struct line_bundle_atom));
}
