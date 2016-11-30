/*
 * TODO:
 *    - on window resize, ensure current top content line is still visible as
 *      first row.
 *    - page up/down support
 *    - handle markdown ?
 */
#include "todo.h"
#include "ui_todo.h"
#include "ui_sheet.h"

#define SHT_INIT_CAT_LEN (8U)

static __nonull(1, 4)
int ui_paint_sheet(struct ui_window const*   window,
                   unsigned int              top,
                   unsigned int const        row_nr,
                   struct line_bundle const* lines)
{
	ut_assert(window);
	ut_assert(row_nr);
	ut_assert(lines);

	unsigned int a;
	unsigned int cnt;
	unsigned int row;
	size_t       sz;

	for (a = 0, cnt = lbndl_atom_count(lines), row = 0;
	     (a < cnt) && (row < top);
	     a++)
		if (!lbndl_atom(lines, a, &sz))
			row++;

	ui_erase_window(window);

	for (row = 0; a < cnt; a++) {
		char const* ln;

		ln = lbndl_atom(lines, a, &sz);
		if (ln) {
			ui_print_window_string(window, ln, sz);
			continue;
		}

		if (++row >= row_nr)
			break;

		ui_move_window_cursor(window, row, 0);
	}

	return 0;
}

int ui_scroll_sheet_up(struct ui_sheet* thiz)
{
	unsigned int const            top = thiz->sht_top;
	struct ui_window const* const win = &thiz->sht_win;
	int                           err;

	if (!top)
		return 0;

	err = ui_paint_sheet(win,
	                     top - 1,
	                     ui_window_height(win),
	                     &thiz->sht_ln);
	if (err)
		return err;

	thiz->sht_top = top - 1;

	return 0;
}


int ui_scroll_sheet_down(struct ui_sheet* thiz)
{
	struct ui_window* const   win = &thiz->sht_win;
	struct line_bundle* const ln = &thiz->sht_ln;
	unsigned int const        top = thiz->sht_top;
	unsigned int              row_nr;
	unsigned int              ln_nr;
	int                       err;
	
	row_nr = ui_window_height(win);
	ln_nr = lbndl_line_count(ln);

	if ((top + ut_min(row_nr, ln_nr - top)) == ln_nr)
		return 0;

	err = ui_paint_sheet(win, top + 1, row_nr, ln);
	if (err)
		return err;

	thiz->sht_top = top + 1;

	return 0;
}

__nonull(1, 2)
int ui_render_sheet(struct ui_sheet* thiz, struct ui_geometry const* geometry)
{
	struct ui_window* const win = &thiz->sht_win;
	unsigned int            cols;
	struct line_bundle*     ln = &thiz->sht_ln;
	int                     err;

	ui_update_window_geometry(win, geometry);

	cols = ui_window_width(win);

	lbndl_clear(ln);

	err = lbndl_push_string(ln, "Title: ", cols);
	if (err)
		return err;
	err = lbndl_push_string(ln, thiz->sht_tle, cols);
	if (err)
		return err;

	if (thiz->sht_prio[0] != '\0') {
		err = lbndl_push_newline(ln);
		if (err)
			return err;
		err = lbndl_push_string(ln, "Priority: ", cols);
		if (err)
			return err;
		err = lbndl_push_string(ln, thiz->sht_prio, cols);
		if (err)
			return err;
	}

	if (dstr_len(&thiz->sht_cats)) {
		err = lbndl_push_newline(ln);
		if (err)
			return err;
		err = lbndl_push_string(ln, "Categories: ", cols);
		if (err)
			return err;
		err = lbndl_push_string(ln, dstr_charp(&thiz->sht_cats), cols);
		if (err)
			return err;
	}

	err = lbndl_push_newline(ln);
	if (err)
		return err;
	err = lbndl_push_string(ln, "Calendar: ", cols);
	if (err)
		return err;
	err = lbndl_push_string(ln, thiz->sht_cal, cols);
	if (err)
		return err;

	if (thiz->sht_loc) {
		err = lbndl_push_newline(ln);
		if (err)
			return err;
		err = lbndl_push_string(ln, "Location: ", cols);
		if (err)
			return err;
		err = lbndl_push_string(ln, thiz->sht_loc, cols);
		if (err)
			return err;
	}

	if (thiz->sht_desc) {
		err = lbndl_push_newline(ln);
		if (err)
			return err;
		err = lbndl_push_newline(ln);
		if (err)
			return err;
		err = lbndl_push_string(ln, thiz->sht_desc, cols);
		if (err)
			return err;
	}

	ui_paint_sheet(win, thiz->sht_top, geometry->h, ln);

	return 0;
}

__nonull(1)
void ui_load_sheet(struct ui_sheet* thiz,
                   unsigned int     index)
{
	ui_assert_sheet(thiz);
	ut_assert(index < todo_repo_count(thiz->sht_todos));

	struct todo* todo;

	thiz->sht_top = 0;

	todo = todo_repo_entry(thiz->sht_todos, index);

	thiz->sht_tle = ui_todo_title(todo);

	ui_todo_priority(todo,
	                 thiz->sht_prio,
	                 sizeof(thiz->sht_prio));

	ui_todo_categories(todo, &thiz->sht_cats);

	thiz->sht_cal = ui_todo_calendar(todo);

	thiz->sht_loc = ui_todo_location(todo);

	thiz->sht_desc = ui_todo_description(todo);
}

__nonull(1, 2)
int ui_init_sheet(struct ui_sheet*  thiz,
                  struct todo_repo* todos)
{
	ut_assert(thiz);
	ut_assert(todos);

	int err;

	err = lbndl_init(&thiz->sht_ln);
	if (err)
		return err;

	err = dstr_init(&thiz->sht_cats, SHT_INIT_CAT_LEN);
	if (err)
		goto lines;

	/* Create window leaving enought place for status bar. */
	err = ui_init_full_window(&thiz->sht_win);
	if (err)
		goto cats;

	thiz->sht_top = 0;
	thiz->sht_todos = todos;

	return 0;

cats:
	dstr_fini(&thiz->sht_cats);
lines:
	lbndl_fini(&thiz->sht_ln);

	return err;
}

__nonull(1)
void ui_fini_sheet(struct ui_sheet const* thiz)
{
	ui_assert_sheet(thiz);

	ui_fini_window(&thiz->sht_win);
	dstr_fini(&thiz->sht_cats);
	lbndl_fini(&thiz->sht_ln);
}
