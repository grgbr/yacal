#ifndef _UI_SHEET_H
#define _UI_SHEET_H

#include "ui.h"
#include "string.h"

struct todo_repo;

struct ui_sheet {
	unsigned int       sht_index;
	char const*        sht_tle;
	char               sht_prio[UI_TODO_PRIO_LEN + 1];
	struct dstr        sht_cats;
	char const*        sht_cal;
	char const*        sht_loc;
	char const*        sht_desc;
	struct ui_window   sht_win;
	struct todo_repo*  sht_todos;
};

#define ui_assert_sheet(_sht)     \
	ut_assert(_sht);          \
	ut_assert(_sht->sht_todos)

static inline __nonull(1)
void ui_show_sheet(struct ui_sheet const* thiz)
{
	ui_assert_sheet(thiz);

	ui_show_window(&thiz->sht_win);
}

extern void ui_render_sheet(struct ui_sheet*,
                             struct ui_geometry const*,
                             unsigned int) __nonull(1, 2);
extern int  ui_init_sheet(struct ui_sheet*, struct todo_repo*) __nonull(1, 2);
extern void ui_fini_sheet(struct ui_sheet const*) __nonull(1);

#endif
