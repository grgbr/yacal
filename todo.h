#ifndef _TODO_H
#define _TODO_H

#include <stdbool.h>
#include "vdir.h"
#include "vector.h"
#include "utils.h"

static inline
icalerrorenum todo_errno(void)
{
	return icalerrno;
}

static inline
char const* todo_strerror(icalerrorenum error)
{
	return icalerror_strerror(error);
}

typedef int (todo_sort_fn)(void const*, void const*);

struct todo_repo {
	struct vector  todo_store;
	todo_sort_fn  *todo_sort;
	char const*    todo_name;
	struct vdir    todo_vdir;
};

struct todo_category_repo {
	bool          cat_dirty;
	struct vector cat_store;
};

struct todo {
	icalcomponent*            todo_cpnt;
	struct todo_category_repo todo_cats;
	struct todo_repo const*   todo_repo;
};

#define todo_assert(_todo)           \
	ut_assert(_todo);            \
	ut_assert(_todo->todo_cpnt); \
	ut_assert(_todo->todo_repo)

static inline __nonull(1)
char const* todo_summary(struct todo const* thiz)
{
	todo_assert(thiz);

	return icalcomponent_get_summary(thiz->todo_cpnt);
}

static inline __nonull(1) __nothrow __pure
char const* todo_calendar(struct todo const* thiz)
{
	todo_assert(thiz);

	return thiz->todo_repo->todo_name;
}

static inline __nonull(1)
char const* todo_location(struct todo const* thiz)
{
	todo_assert(thiz);

	return icalcomponent_get_location(thiz->todo_cpnt);
}

static inline __nonull(1)
char const* todo_description(struct todo const* thiz)
{
	todo_assert(thiz);

	return icalcomponent_get_description(thiz->todo_cpnt);
}

extern int         todo_priority(struct todo const*) __nonull(1);
extern int         todo_category_count(struct todo*) __nonull(1);
extern char const* todo_category_entry(struct todo*, unsigned int) __nonull(1);

static inline __nonull(1) __nothrow __pure
unsigned int todo_repo_count(struct todo_repo const* thiz)
{
	ut_assert(thiz);

	return vect_count(&thiz->todo_store);
}

extern struct todo* todo_repo_entry(struct todo_repo const*,
                                    unsigned int) __nonull(1) __nothrow;
extern void         todo_setup_repo_sort(struct todo_repo*,
                                         todo_sort_fn*) __nonull(1, 2);
extern int          todo_load_repo(struct todo_repo*) __nonull(1);
extern int          todo_init_repo(struct todo_repo*,
                                   const char*,
                                   const char*,
                                   todo_sort_fn*) __nonull(1, 2, 3, 4);
extern void         todo_fini_repo(struct todo_repo const*) __nonull(1);

extern int          todo_sortby_summary(void const*,
                              void const*) __nonull(1, 2);

#endif
