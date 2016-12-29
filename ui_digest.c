#include <string.h>
#include <errno.h>
#include "todo.h"
#include "ui_todo.h"
#include "ui_digest.h"

#define DGST_MIN_TITLE_LEN (1U)
#define DGST_CNT_LEN       (4U)

static
int ui_paint_digest_todo(unsigned int entry, unsigned int row, void* data)
{
	struct ui_digest* const       dgst = data;
	struct todo const*            todo;
	struct ui_window const* const win = &dgst->dgst_win;
	struct ui_field* const        flds = dgst->dgst_flds;

	todo = todo_repo_entry(dgst->dgst_todos, entry);

	/* Move cursor to begining of current row. */
	ui_move_window_cursor(win, row, 0);

	/* Display todo index / count field. */
	ui_render_field(&flds[DGST_CNT_FLD], win, "%3u.", entry + 1);

	/* Display todo title / summary. */
	ui_render_string_field(&flds[DGST_TITLE_FLD], win, ui_todo_title(todo));

	return 0;
}

static
int ui_select_digest_todo(unsigned int row, void* data)
{
	struct ui_digest const* const dgst = data;
	struct ui_window const* const win = &dgst->dgst_win;
	attr_t const                  attr = dgst->dgst_attr;
	short const                   clr = dgst->dgst_color;

	/*
	 * Cursor is pointing to currently selected line : deselect it using
	 * window's default display attributes and colors pair.
	 */
	ui_setup_current_line_attrs(win, attr, clr);

	/*
	 * Move cursor to begining of line corresponding to index then highlight
	 * entire line.
	 */
	ui_setup_line_attrs(win, row, attr | A_REVERSE, clr);

	return 0;
}

__nonull(1)
bool ui_scroll_digest_up(struct ui_digest* thiz)
{
	ui_assert_digest(thiz);

	struct scroller_operation     op;
	struct ui_window const* const win = &thiz->dgst_win;

	if (!thiz->dgst_select)
		return false;

	scrl_render(&thiz->dgst_scrl,
	            thiz->dgst_select - 1,
	            todo_repo_count(thiz->dgst_todos),
	            ui_window_width(win),
	            ui_window_height(win),
	            &op);
	
	scrl_paint(&op, ui_paint_digest_todo, ui_select_digest_todo, thiz);

	thiz->dgst_select--;

	return true;
}

__nonull(1)
bool ui_scroll_digest_down(struct ui_digest* thiz)
{
	ui_assert_digest(thiz);

	unsigned int const            cnt = todo_repo_count(thiz->dgst_todos);
	unsigned int const            select = thiz->dgst_select + 1;
	struct ui_window const* const win = &thiz->dgst_win;
	struct scroller_operation     op;

	if (select >= cnt)
		return false;

	scrl_render(&thiz->dgst_scrl,
	            select,
	            cnt,
	            ui_window_width(win),
	            ui_window_height(win),
	            &op);
	
	scrl_paint(&op, ui_paint_digest_todo, ui_select_digest_todo, thiz);

	thiz->dgst_select = select;

	return true;
}

__nonull(1, 2)
void ui_render_digest(struct ui_digest*         thiz,
                      struct ui_geometry const* geometry)
{
	ui_assert_digest(thiz);

	struct scroller_operation op;
	struct scroller* const    scrl = &thiz->dgst_scrl;

	ui_update_window_geometry(&thiz->dgst_win, geometry);

	if (scrl_columns_nr(scrl) != geometry->w)
		ui_adjust_available_field_width(thiz->dgst_flds,
		                                geometry->w,
		                                ut_array_count(thiz->dgst_flds));

	scrl_render(scrl,
	            thiz->dgst_select,
	            todo_repo_count(thiz->dgst_todos),
	            geometry->w,
	            geometry->h,
	            &op);

	scrl_paint(&op, ui_paint_digest_todo, ui_select_digest_todo, thiz);
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

	scrl_reset(&thiz->dgst_scrl, 0);

	thiz->dgst_select = 0;
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
