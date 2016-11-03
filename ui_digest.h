#ifndef _UI_DIGEST_H
#define _UI_DIGEST_H

#include "ui.h"

struct todo_repo;

enum ui_digest_field {
	DGST_CNT_FLD,
	DGST_TITLE_FLD,
	DGST_NR_FLD
};

struct ui_digest {
	struct ui_window      dgst_win;
	attr_t                dgst_attr;
	short                 dgst_color;
	struct ui_field       dgst_flds[DGST_NR_FLD];
	struct ui_scroll_list dgst_scrl;
	struct todo_repo*     dgst_todos;
};

#define ui_assert_digest(_dgst)     \
	ut_assert(_dgst);           \
	ut_assert(_dgst->dgst_todos)

static inline __nonull(1)
void ui_show_digest(struct ui_digest const* thiz)
{
	ui_assert_digest(thiz);

	ui_show_window(&thiz->dgst_win);
}

extern void ui_render_digest(struct ui_digest*,
                             struct ui_geometry const*,
                             unsigned int) __nonull(1, 2);
extern void ui_load_digest(struct ui_digest*) __nonull(1);
extern int  ui_init_digest(struct ui_digest*,
                           struct ui_geometry const*,
                           struct todo_repo*) __nonull(1, 2, 3);
extern void ui_fini_digest(struct ui_digest const*) __nonull(1);

#endif
