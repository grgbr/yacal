#ifndef _PAGER_H
#define _PAGER_H

#include <stdbool.h>
#include "vector.h"
#include "utils.h"

typedef int (pager_render_atom_fn)(char const*, size_t, void*);
typedef int (pager_render_line_fn)(char const*,
                                   size_t,
                                   unsigned int,
                                   unsigned int,
                                   void*);

enum pager_state {
	PG_INIT_STAT,
	PG_SCAN_STAT,
	PG_FILL_STAT,
	PG_BREAK_STAT
};

struct pager {
	struct vector         pg_atms;
	struct vector         pg_lns;
	pager_render_line_fn* pg_render;
	enum pager_state      pg_stat;
	size_t                pg_max_len;
	size_t                pg_sz;
};

#define pg_assert(_pager) \
	ut_assert(_pager)

static inline __nonull(1) __nothrow __pure
size_t pg_max_len(struct pager const* thiz)
{
	pg_assert(thiz);

	return thiz->pg_max_len;
}

static inline __nonull(1) __nothrow __pure
size_t pg_line_count(struct pager const* thiz)
{
	pg_assert(thiz);

	return vect_count(&thiz->pg_lns);
}


extern int          pg_paint(struct pager*, unsigned int, unsigned int, void*)
                    __nonull(1);
extern int          pg_feed_string(struct pager*,
                                   char const*,
                                   pager_render_atom_fn*)
                    __nonull(1, 2, 3) __nothrow;
extern int          pg_feed_line(struct pager*, unsigned int)
                    __nonull(1) __nothrow;
extern bool         pg_dress(struct pager*, size_t, pager_render_line_fn*)
                    __nonull(1, 3) __nothrow;
extern void         pg_clear(struct pager*)
                    __nonull(1) __nothrow;
extern unsigned int pg_visible_level(struct pager const*,
                                     unsigned int,
                                     unsigned int)
                    __nonull(1) __nothrow __pure;
extern int          pg_init(struct pager*)
                    __nonull(1) __nothrow;
extern void         pg_fini(struct pager*)
                    __nonull(1) __nothrow;

#endif
