#ifndef _UI_STATUS_H
#define _UI_STATUS_H

#include "dstr.h"
#include "ui.h"
#include "ui_todo.h"

struct todo;
struct todo_repo;

enum ui_status_field {
	STAT_CAL_FLD,
	STAT_CAT_FLD,
	STAT_PRIO_FLD,
	STAT_CNT_FLD,
	STAT_NR_FLD
};

struct ui_status {
	unsigned int            stat_index;
	struct ui_field         stat_flds[STAT_NR_FLD];
	char const*             stat_cal;
	struct dstr             stat_cat;
	char                    stat_prio[UI_TODO_PRIO_LEN + 1];
	char                    stat_cnt[UI_TODO_PERCENT_LEN + 1];
	struct ui_window        stat_win;
	struct todo_repo const* stat_todos;
};

#define ui_assert_status(_stat)     \
	ut_assert(_stat);           \
	ut_assert(_stat->stat_todos)

static inline __nonull(1)
void ui_show_status(struct ui_status const* thiz)
{
	ui_assert_status(thiz);

	ui_show_window(&thiz->stat_win);
}

extern void ui_render_status(struct ui_status*,
                             struct ui_geometry const*,
                             unsigned int) __nonull(1, 2);
extern void ui_load_status(struct ui_status*) __nonull(1);
extern int  ui_init_status(struct ui_status*,
                           struct ui_geometry const*,
                           struct todo_repo const*) __nonull(1, 2, 3);
extern void ui_fini_status(struct ui_status const*) __nonull(1);

#endif
