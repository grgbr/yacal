#include <string.h>
#include <errno.h>
#include "todo.h"
#include "ui_todo.h"
#include "ui_digest.h"

#define DGST_MIN_TITLE_LEN (1U)
#define DGST_CNT_LEN       (4U)

static __nonull(1)
void ui_paint_digest(struct ui_digest* thiz,
                     unsigned int      top_entry,
                     unsigned int      start_row,
                     unsigned int      select_row,
                     unsigned int      count)
{
	struct todo_repo const* const todos = thiz->dgst_todos;
	unsigned int                  c;
	struct ui_window const* const win = &thiz->dgst_win;
	struct ui_field* const        flds = thiz->dgst_flds;
	attr_t const                  attr = thiz->dgst_attr;
	short const                   clr = thiz->dgst_color;

	for (c = 0; c < count; c++) {
		struct todo const* todo;

		todo = todo_repo_entry(todos, top_entry + c);

		/* Move cursor to begining of current row. */
		ui_move_window_cursor(win, start_row + c, 0);

		/* Display todo index / count field. */
		ui_render_field(&flds[DGST_CNT_FLD],
		                win,
		                "%3u.",
		                top_entry + c + 1);

		/* Display todo title / summary. */
		ui_render_string_field(&flds[DGST_TITLE_FLD],
		                       win,
		                       ui_todo_title(todo));
	}

	/*
	 * Cursor is pointing to currently selected line : deselect it using
	 * window's default display attributes and colors pair.
	 */
	ui_setup_current_line_attrs(win, attr, clr);

	/*
	 * Move cursor to begining of line corresponding to index then highlight
	 * entire line.
	 */
	ui_setup_line_attrs(win, select_row, attr | A_REVERSE, clr);
}

__nonull(1)
bool ui_scroll_digest_up(struct ui_digest* thiz)
{
	unsigned int                  select = thiz->dgst_select;
	struct ui_geometry            geom;
	struct ui_scroll_list_refresh refresh;

	if (!select)
		return false;

	thiz->dgst_select = select - 1;

	geom.h = ui_window_height(&thiz->dgst_win);
	geom.w = ui_window_width(&thiz->dgst_win);

	select = ui_render_scroll_list(&thiz->dgst_scrl,
	                               thiz->dgst_select,
	                               todo_repo_count(thiz->dgst_todos),
	                               &geom,
	                               &refresh);

	ui_paint_digest(thiz,
	                refresh.scrl_top,
	                refresh.scrl_row,
	                select,
	                refresh.scrl_cnt);

	return true;
}

__nonull(1)
bool ui_scroll_digest_down(struct ui_digest* thiz)
{
	unsigned int                  select = thiz->dgst_select + 1;
	unsigned int const            cnt = todo_repo_count(thiz->dgst_todos);
	struct ui_geometry            geom;
	struct ui_scroll_list_refresh refresh;

	if (select >= cnt)
		return false;

	thiz->dgst_select = select;

	geom.h = ui_window_height(&thiz->dgst_win);
	geom.w = ui_window_width(&thiz->dgst_win);

	select = ui_render_scroll_list(&thiz->dgst_scrl,
	                               select,
	                               cnt,
	                               &geom,
	                               &refresh);

	ui_paint_digest(thiz,
	                refresh.scrl_top,
	                refresh.scrl_row,
	                select,
	                refresh.scrl_cnt);

	return true;
}

__nonull(1, 2)
void ui_render_digest(struct ui_digest*         thiz,
                      struct ui_geometry const* geometry)
{
	ui_assert_digest(thiz);

	struct ui_scroll_list_refresh refresh;
	unsigned int                  select;

	ui_update_window_geometry(&thiz->dgst_win, geometry);

	if (ui_scroll_list_width_changed(&thiz->dgst_scrl, geometry))
		ui_adjust_available_field_width(thiz->dgst_flds,
		                                geometry->w,
		                                ut_array_count(thiz->dgst_flds));

	select = ui_render_scroll_list(&thiz->dgst_scrl,
	                               thiz->dgst_select,
	                               todo_repo_count(thiz->dgst_todos),
	                               geometry,
	                               &refresh);

	ui_paint_digest(thiz,
	                refresh.scrl_top,
	                refresh.scrl_row,
	                select,
	                refresh.scrl_cnt);
}

__nonull(1)
void ui_load_digest(struct ui_digest* thiz)
{
	ui_assert_digest(thiz);

	struct ui_field* const        flds = thiz->dgst_flds;
	struct todo_repo const* const todos = thiz->dgst_todos;
	unsigned int     cnt;
	unsigned int     t;

	ui_reset_optimal_field_width(&flds[DGST_TITLE_FLD], 0);

	cnt = todo_repo_count(todos);
	for (t = 0; t < cnt; t++) {
		char const* str;

		str = ui_todo_title(todo_repo_entry(todos, t));

		ui_adjust_optimal_field_width(&flds[DGST_TITLE_FLD],
		                              strlen(str));
	}

	ui_reset_scroll_list(&thiz->dgst_scrl);

	thiz->dgst_select = ut_min(thiz->dgst_select, cnt);
}

__nonull(1, 2)
int ui_init_digest(struct ui_digest* thiz, struct todo_repo const* todos)
{
	ut_assert(thiz);
	ut_assert(todos);

	int err;

	/* Create window leaving enought place for status bar. */
	err = ui_init_full_window(&thiz->dgst_win);
	if (err)
		return err;

	/* Retrieve window's current attributes and colors pair. */
	ui_fetch_window_attrs(&thiz->dgst_win,
	                      &thiz->dgst_attr,
	                      &thiz->dgst_color);

	ui_init_fixed_field(&thiz->dgst_flds[DGST_CNT_FLD], DGST_CNT_LEN);
	ui_init_variable_field(&thiz->dgst_flds[DGST_TITLE_FLD],
	                       DGST_MIN_TITLE_LEN);

	thiz->dgst_select = 0;
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
