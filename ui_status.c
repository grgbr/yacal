#include "string.h"
#include "todo.h"
#include "ui_status.h"

#define STAT_MIN_CAL_LEN (6U)
#define STAT_MIN_CAT_LEN (6U)

static __nonull(1, 2, 3)
void ui_paint_status(struct ui_status const* thiz,
                     struct ui_field const*  fields,
                     struct ui_window const* window)
{
	/* Move cursor to begining of status bar. */
	ui_move_window_cursor(window, 0, 0);

	ui_render_string_field(&fields[STAT_CAL_FLD], window, thiz->stat_cal);
	ui_render_string_field(&fields[STAT_CAT_FLD],
	                       window,
	                       dstr_charp(&thiz->stat_cat));
	ui_render_string_field(&fields[STAT_PRIO_FLD], window, thiz->stat_prio);
	ui_render_string_field(&fields[STAT_CNT_FLD], window, thiz->stat_cnt);
}

__nonull(1, 2)
void ui_render_status(struct ui_status*         thiz,
                      struct ui_geometry const* geometry)
{
	ui_assert_status(thiz);

	struct ui_field* const        flds = thiz->stat_flds;
	struct ui_window const* const win = &thiz->stat_win;

	ui_update_window_geometry(win, geometry);

	ui_adjust_available_field_width(flds,
	                                ui_window_width(win),
	                                ut_array_count(thiz->stat_flds));

	ui_paint_status(thiz, flds, win);
}

__nonull(1) __leaf
void ui_select_status(struct ui_status* thiz,
                      unsigned int      index)
{
	ui_assert_status(thiz);

	struct todo_repo const* const todos = thiz->stat_todos;
	struct todo*                  todo;

	todo = todo_repo_entry(todos, index);

	thiz->stat_cal = ui_todo_calendar(todo);

	ui_todo_categories(todo, &thiz->stat_cat);

	ui_todo_priority(todo, thiz->stat_prio, sizeof(thiz->stat_prio));

	ui_todo_percent(thiz->stat_cnt,
	                index + 1,
	                todo_repo_count(todos),
	                sizeof(thiz->stat_cnt));
}

__nonull(1)
void ui_load_status(struct ui_status* thiz, unsigned int index)
{
	ui_assert_status(thiz);

	struct ui_field* const        flds = thiz->stat_flds;
	struct todo_repo const* const todos = thiz->stat_todos;
	unsigned int                  cnt;
	unsigned int                  t;
	struct todo*                  todo;

	ui_reset_optimal_field_width(&flds[STAT_CAL_FLD], 0);
	ui_reset_optimal_field_width(&flds[STAT_CAT_FLD], 0);

	cnt = todo_repo_count(todos);
	for (t = 0; t < cnt; t++) {
		todo = todo_repo_entry(todos, t);

		ui_adjust_optimal_field_width(&flds[STAT_CAL_FLD],
		                              strlen(ui_todo_calendar(todo)));

		ui_todo_categories(todo, &thiz->stat_cat);
		ui_adjust_optimal_field_width(&flds[STAT_CAT_FLD],
		                              dstr_len(&thiz->stat_cat));
	}

	ui_select_status(thiz, index);
}

__nonull(1, 2) __leaf
int ui_init_status(struct ui_status* thiz, struct todo_repo const* todos)
{
	ut_assert(thiz);
	ut_assert(todos);

	attr_t attrs;
	short  clr;
	int    err;

	err = dstr_init(&thiz->stat_cat, STAT_MIN_CAT_LEN);
	if (err)
		return err;

	/* Create status bar window. */
	err = ui_init_full_window(&thiz->stat_win);
	if (err) {
		dstr_fini(&thiz->stat_cat);
		return err;
	}

	ui_fetch_window_attrs(&thiz->stat_win, &attrs, &clr);
	ui_setup_window_attrs(&thiz->stat_win, attrs | A_REVERSE, clr);

	ui_init_variable_field(&thiz->stat_flds[STAT_CAL_FLD],
	                       STAT_MIN_CAL_LEN);
	ui_init_variable_field(&thiz->stat_flds[STAT_CAT_FLD],
	                       STAT_MIN_CAT_LEN);
	ui_init_fixed_field(&thiz->stat_flds[STAT_PRIO_FLD],
	                    UI_TODO_PRIO_LEN);
	ui_init_fixed_field(&thiz->stat_flds[STAT_CNT_FLD],
	                    UI_TODO_PERCENT_LEN);

	thiz->stat_todos = todos;

	return 0;
}

__nonull(1)
void ui_fini_status(struct ui_status const* thiz)
{
	ui_assert_status(thiz);

	ui_fini_field(&thiz->stat_flds[STAT_CAL_FLD]);
	ui_fini_field(&thiz->stat_flds[STAT_CAT_FLD]);
	ui_fini_field(&thiz->stat_flds[STAT_PRIO_FLD]);
	ui_fini_field(&thiz->stat_flds[STAT_CNT_FLD]);

	ui_fini_window(&thiz->stat_win);

	dstr_fini(&thiz->stat_cat);
}
