#include <string.h>
#include <errno.h>
#include "todo.h"
#include "ui_todo.h"
#include "ui_digest.h"

#define DGST_MIN_TITLE_LEN (1U)
#define DGST_CNT_LEN       (4U)

__nonull(1, 2)
void ui_render_digest(struct ui_digest*         thiz,
                      struct ui_geometry const* geometry,
                      unsigned int              index)
{
	ui_assert_digest(thiz);

	struct ui_field* const        flds = thiz->dgst_flds;
	struct ui_window* const       win = &thiz->dgst_win;
	struct todo_repo const* const todos = thiz->dgst_todos;
	struct ui_scroll_list_refresh refresh;
	unsigned int                  c;

	ui_update_window_geometry(win, geometry);

	if (ui_scroll_list_width_changed(&thiz->dgst_scrl, geometry))
		ui_adjust_available_field_width(flds,
		                                geometry->w,
		                                ut_array_count(thiz->dgst_flds));

	index = ui_render_scroll_list(&thiz->dgst_scrl,
	                              index,
	                              todo_repo_count(todos),
	                              geometry,
	                              &refresh);

	for (c = 0; c < refresh.scrl_cnt; c++) {
		struct todo const* todo;

		todo = todo_repo_entry(todos, refresh.scrl_top + c);

		/* Move cursor to begining of current row. */
		ui_move_window_cursor(win, refresh.scrl_row + c, 0);

		/* Display todo index / count field. */
		ui_render_field(&flds[DGST_CNT_FLD],
		                win,
		                "%3u.",
		                refresh.scrl_top + c + 1);

		/* Display todo title / summary. */
		ui_render_string_field(&flds[DGST_TITLE_FLD],
		                       win,
		                       ui_todo_title(todo));
	}

	/*
	 * Cursor is pointing to currently selected line : deselect it using
	 * window's default display attributes and colors pair.
	 */
	ui_setup_current_line_attrs(win, thiz->dgst_attr, thiz->dgst_color);

	/*
	 * Move cursor to begining of line corresponding to index then highlight
	 * entire line.
	 */
	ui_setup_line_attrs(win,
	                    index,
	                    thiz->dgst_attr | A_REVERSE,
	                    thiz->dgst_color);
}

__nonull(1)
void ui_load_digest(struct ui_digest* thiz)
{
	ui_assert_digest(thiz);

	unsigned int     cnt;
	unsigned int     t;

	ui_reset_optimal_field_width(&thiz->dgst_flds[DGST_TITLE_FLD], 0);

	cnt = todo_repo_count(thiz->dgst_todos);
	for (t = 0; t < cnt; t++) {
		char const* str;

		str = ui_todo_title(todo_repo_entry(thiz->dgst_todos, t));

		ui_adjust_optimal_field_width(&thiz->dgst_flds[DGST_TITLE_FLD],
		                              strlen(str));
	}

	ui_reset_scroll_list(&thiz->dgst_scrl);
}

__nonull(1, 2, 3)
int ui_init_digest(struct ui_digest*         thiz,
                   struct ui_geometry const* geometry,
                   struct todo_repo*         todos)
{
	ut_assert(thiz);
	ut_assert(todos);

	int err;

	/* Create window leaving enought place for status bar. */
	err = ui_init_window(&thiz->dgst_win, geometry);
	if (err)
		return err;

	/* Retrieve window's current attributes and colors pair. */
	ui_fetch_window_attrs(&thiz->dgst_win,
	                      &thiz->dgst_attr,
	                      &thiz->dgst_color);

	ui_init_fixed_field(&thiz->dgst_flds[DGST_CNT_FLD], DGST_CNT_LEN);
	ui_init_variable_field(&thiz->dgst_flds[DGST_TITLE_FLD],
	                       DGST_MIN_TITLE_LEN);

	thiz->dgst_todos = todos;

	return 0;
}

__nonull(1)
void ui_fini_digest(struct ui_digest const* thiz)
{
	ui_assert_digest(thiz);

	ui_fini_field(&thiz->dgst_flds[DGST_TITLE_FLD]);
	ui_fini_field(&thiz->dgst_flds[DGST_CNT_FLD]);

	ui_fini_window(&thiz->dgst_win);
}
