#include <errno.h>
#include "vector.h"

static __nonull(1)
int vect_grow(struct vector* thiz)
{
	vect_assert(thiz);

	if (thiz->vect_cnt == thiz->vect_sz) {
		const size_t  sz = 2 * thiz->vect_sz;
		char         *arr;

		arr = realloc(thiz->vect_arr, sz * thiz->vect_ent_sz);
		if (!arr)
			return -ENOMEM;

		thiz->vect_arr = arr;
		thiz->vect_sz = sz;
	}

	return 0;
}

__nonull(1)
void vect_shrink(struct vector* thiz)
{
	vect_assert(thiz);

	if (thiz->vect_sz > VECT_INIT_SZ) {
		unsigned int sz;

		sz = ut_max(1U << ut_upper_pow2(thiz->vect_cnt), VECT_INIT_SZ);
		if (thiz->vect_sz > sz) {
			char *arr;

			arr = realloc(thiz->vect_arr, sz * thiz->vect_ent_sz);
			if (!arr)
				return;

			thiz->vect_arr = arr;
			thiz->vect_sz = sz;
		}
	}
}

__nonull(1)
char* vect_push(struct vector* thiz)
{
	vect_assert(thiz);

	int err;

	err = vect_grow(thiz);
	if (err) {
		errno = -err;
		return NULL;
	}

	return thiz->vect_arr + (thiz->vect_cnt++ * thiz->vect_ent_sz);
}

__nonull(1)
void vect_pop(struct vector* thiz)
{
	vect_assert(thiz);
	ut_assert(thiz->vect_cnt);

	thiz->vect_cnt--;
	vect_shrink(thiz);
}

__nonull(1)
int vect_init(struct vector* thiz, size_t entry_size)
{
	ut_assert(thiz);
	ut_assert(entry_size);

	thiz->vect_arr = malloc(VECT_INIT_SZ * entry_size);
	if (!thiz->vect_arr)
		return -ENOMEM;

	thiz->vect_cnt = 0;
	thiz->vect_sz = VECT_INIT_SZ;
	thiz->vect_ent_sz = entry_size;

	return 0;
}
