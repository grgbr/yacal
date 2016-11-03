#include <string.h>
#include <errno.h>
#include "todo.h"

/******************************************************************************
 * Todo categories handling.
 ******************************************************************************/

static __nonull(1, 2) __pure
int todo_sortby_category_name(void const* a, void const* b)
{
	char const* const* const aname = a;
	char const* const* const bname = b;

	ut_assert(*aname);
	ut_assert(*bname);

	return strcmp(*aname, *bname);
}

static __nonull(1)
char const* todo_begin_ical_category(icalcomponent* component)
{
	ut_assert(component);

	icalproperty* prop;

	prop = icalcomponent_get_first_property(component,
	                                        ICAL_CATEGORIES_PROPERTY);
	if (!prop)
		return NULL;

	return icalproperty_get_categories(prop);
}

static __nonull(1)
char const* todo_next_ical_category(icalcomponent* component)
{
	ut_assert(component);

	icalproperty* prop;

	prop = icalcomponent_get_next_property(component,
	                                       ICAL_CATEGORIES_PROPERTY);
	if (!prop)
		return NULL;

	return icalproperty_get_categories(prop);
}

static __nonull(1, 2)
int todo_doload_category_repo(struct todo_category_repo* thiz,
                              struct todo const*         todo)
{
	icalcomponent* const cpnt = todo->todo_cpnt;
	char const*          name;
	int                  ret;
	unsigned int         cnt;

	vect_clear(&thiz->cat_store);

	for (name = todo_begin_ical_category(cpnt);
	     name;
	     name = todo_next_ical_category(cpnt)) {
		char const** cat;

		cat = (char const**)vect_push(&thiz->cat_store);
		if (!cat) {
			ret = -errno;
			goto shrink;
		}

		*cat = name;
	}

	ret = 0;

	cnt = vect_count(&thiz->cat_store);
	if (!cnt)
		goto shrink;

	qsort(vect_array(&thiz->cat_store),
	      cnt,
	      vect_entry_size(&thiz->cat_store),
	      todo_sortby_category_name);

shrink:
	vect_shrink(&thiz->cat_store);

	return ret;
}

static __nonull(1, 2)
int todo_load_category_repo(struct todo_category_repo* thiz,
                            struct todo const*         todo)
{
	ut_assert(thiz);
	ut_assert(todo);

	int err;

	if (!thiz->cat_dirty)
		return 0;

	err = todo_doload_category_repo(thiz, todo);
	if (!err) {
		thiz->cat_dirty = false;
		return 0;
	}

	return err;
}

static __nonull(1)
int todo_category_repo_count(struct todo_category_repo* thiz,
                             struct todo const*         todo)
{
	int err;

	err = todo_load_category_repo(thiz, todo);
	if (err)
		return err;

	return (int)vect_count(&thiz->cat_store);
}

static __nonull(1)
char const* todo_category_repo_entry(struct todo_category_repo* thiz,
                                     struct todo const*         todo,
                                     unsigned int               index)
{
	int err;

	err = todo_load_category_repo(thiz, todo);
	if (err) {
		errno = -err;
		return NULL;
	}

	if (index >= vect_count(&thiz->cat_store)) {
		errno = ENOENT;
		return NULL;
	}

	return *(char const**)vect_entry(&thiz->cat_store, index);
}

static __nonull(1)
int todo_init_category_repo(struct todo_category_repo* thiz)
{
	ut_assert(thiz);

	thiz->cat_dirty = true;

	return vect_init(&thiz->cat_store, sizeof(char**));
}

static __nonull(1)
void todo_fini_category_repo(struct todo_category_repo* thiz)
{
	ut_assert(thiz);

	vect_fini(&thiz->cat_store);
}

/******************************************************************************
 * Todo (task) handling.
 ******************************************************************************/

__nonull(1)
int todo_priority(struct todo const* thiz)
{
	todo_assert(thiz);

	icalproperty* prop;

	prop = icalcomponent_get_first_property(thiz->todo_cpnt,
	                                        ICAL_PRIORITY_PROPERTY);
	if (!prop)
		return -ENOENT;

	return icalproperty_get_priority(prop);
}

__nonull(1)
int todo_category_count(struct todo* thiz)
{
	todo_assert(thiz);

	return todo_category_repo_count(&thiz->todo_cats, thiz);
}

__nonull(1)
char const* todo_category_entry(struct todo* thiz, unsigned int index)
{
	todo_assert(thiz);

	return todo_category_repo_entry(&thiz->todo_cats, thiz, index);
}

static __nonull(1, 2, 3)
int todo_init(struct todo*            thiz,
              icalcomponent*          component,
              struct todo_repo const* repo)
{
	ut_assert(thiz);
	ut_assert(component);
	ut_assert(repo);

	int err;

	err = todo_init_category_repo(&thiz->todo_cats);
	if (err)
		return err;

	thiz->todo_cpnt = component;
	thiz->todo_repo = repo;

	return 0;
}

static __nonull(1)
void todo_fini(struct todo* thiz)
{
	todo_assert(thiz);

	todo_fini_category_repo(&thiz->todo_cats);
}

/******************************************************************************
 * Todo repository handling.
 ******************************************************************************/

__nonull(1, 2)
int todo_sortby_summary(void const* a, void const* b)
{
	struct todo const* const atodo = a;
	struct todo const* const btodo = b;
	char const*              asum;
	char const*              bsum;

	todo_assert(atodo);
	todo_assert(btodo);

	asum = todo_summary(atodo);
	bsum = todo_summary(btodo);

	if (asum && bsum)
		return strcmp(asum, bsum);

	if (!(asum || bsum))
		return 0;

	if (!asum)
		return -1;

	return 1;
}

__nonull(1) __nothrow
struct todo* todo_repo_entry(struct todo_repo const* thiz,
                             unsigned int            index)
{
	ut_assert(thiz);

	if (index >= vect_count(&thiz->todo_store)) {
		errno = ENOENT;
		return NULL;
	}

	return (struct todo*)vect_entry(&thiz->todo_store, index);
}

static __nonull(1)
void todo_sort_repo(struct todo_repo const* thiz)
{
	ut_assert(thiz);
	ut_assert(thiz->todo_sort);

	unsigned int const cnt = vect_count(&thiz->todo_store);

	if (!cnt)
		return;

	qsort(vect_array(&thiz->todo_store),
	      cnt,
	      vect_entry_size(&thiz->todo_store),
	      thiz->todo_sort);
}

__nonull(1, 2)
void todo_setup_repo_sort(struct todo_repo* thiz, todo_sort_fn* sort)
{
	ut_assert(thiz);
	ut_assert(sort);

	if (sort == thiz->todo_sort)
		return;

	thiz->todo_sort = sort;
	todo_sort_repo(thiz);
}

__nonull(1)
int todo_load_repo(struct todo_repo* thiz)
{
	ut_assert(thiz);
	ut_assert(thiz->todo_sort);

	struct vdir_iterator  iter;
	icalcomponent*        cpnt;
	int                   ret;
	unsigned int          t;

	for (t = 0; t < vect_count(&thiz->todo_store); t++)
		todo_fini((struct todo*)vect_entry(&thiz->todo_store, t));

	vect_clear(&thiz->todo_store);

	for (cpnt = vdir_begin_iterator(&iter, &thiz->todo_vdir);
	     cpnt;
	     cpnt = vdir_next_component(&iter)) {
		if (icalcomponent_isa(cpnt) == ICAL_VTODO_COMPONENT) {
			struct todo* todo;

			todo = (struct todo*)vect_push(&thiz->todo_store);
			if (!todo) {
				ret = -errno;
				goto shrink;
			}

			ret = todo_init(todo, cpnt, thiz);
			if (ret) {
				vect_pop(&thiz->todo_store);
				goto shrink;
			}
		}
	}

	ret = 0;

	todo_sort_repo(thiz);

shrink:
	vect_shrink(&thiz->todo_store);

	return ret;
}

__nonull(1, 2, 3, 4)
int todo_init_repo(struct todo_repo* thiz,
                   char const*       name,
                   const char*       vdir_path,
                   todo_sort_fn*     sort)
{
	ut_assert(thiz);
	ut_assert(vdir_path && (vdir_path[0] != '\0'));
	ut_assert(sort);

	int err;

	err = vect_init(&thiz->todo_store, sizeof(struct todo));
	if (err)
		return err;


	err = vdir_open(&thiz->todo_vdir, vdir_path);
	if (err)
		goto err;

	thiz->todo_sort = sort;
	thiz->todo_name = name;

	return 0;

err:
	vect_fini(&thiz->todo_store);

	return err;
}

__nonull(1)
void todo_fini_repo(struct todo_repo const* thiz)
{
	ut_assert(thiz);

	unsigned int t;

	vdir_close(&thiz->todo_vdir);

	for (t = 0; t < vect_count(&thiz->todo_store); t++)
		todo_fini((struct todo*)vect_entry(&thiz->todo_store, t));

	vect_fini(&thiz->todo_store);
}
