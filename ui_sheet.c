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

static
int ui_render_sheet_line(char const*  string __unused,
                         size_t       length,
                         unsigned int line_no,
                         unsigned int indent,
                         void*        data)
{
	if (length)
		ui_move_window_cursor((struct ui_window*)data, line_no, indent);

	return 0;
}

static
int ui_render_sheet_field(char const* string, size_t length, void* data)
{
	struct ui_window const* const win = (struct ui_window*)data;

	ui_window_attron(win, A_BOLD);
	ui_print_window_string(win, string, length);
	ui_window_attroff(win, A_BOLD);

	return 0;
}

static
int ui_render_sheet_value(char const* string, size_t length, void* data)
{
	ui_print_window_string((struct ui_window*)data, string, length);

	return 0;
}

int ui_scroll_sheet_up(struct ui_sheet* thiz)
{
	unsigned int const      top = thiz->sht_top;
	struct ui_window* const win = &thiz->sht_win;
	int                     err;

	if (!top)
		return 0;

	ui_erase_window(win);

	err = pg_paint(&thiz->sht_pg, top - 1, ui_window_height(win), win);
	if (err)
		return err;

	thiz->sht_top = top - 1;

	return 0;
}


int ui_scroll_sheet_down(struct ui_sheet* thiz)
{
	struct ui_window* const win = &thiz->sht_win;
	unsigned int const      top = thiz->sht_top;
	struct pager* const     pg = &thiz->sht_pg;
	unsigned int            height = ui_window_height(win);
	int                     err;
	
	if ((top + height) >= pg_line_count(pg))
		return 0;

	ui_erase_window(win);

	err = pg_paint(pg, top + 1, height, win);
	if (err)
		return err;

	thiz->sht_top = top + 1;

	return 0;
}

static
int ui_render_sheet_pager(struct ui_sheet* thiz,
                          struct pager*    pager,
                          size_t           width)
{
	int err;

	if (pg_dress(pager, width, ui_render_sheet_line))
		return 0;

	err = pg_feed_string(pager, "Title: ", ui_render_sheet_field);
	if (err)
		return err;
	err = pg_feed_string(pager, thiz->sht_tle, ui_render_sheet_value);
	if (err)
		return err;

	if (thiz->sht_prio[0] != '\0') {
		err = pg_feed_line(pager, 0);
		if (err)
			return err;
		err = pg_feed_string(pager,
		                     "Priority: ",
		                     ui_render_sheet_field);
		if (err)
			return err;
		err = pg_feed_string(pager,
		                     thiz->sht_prio,
		                     ui_render_sheet_value);
		if (err)
			return err;
	}

	if (dstr_len(&thiz->sht_cats)) {
		err = pg_feed_line(pager, 0);
		if (err)
			return err;
		err = pg_feed_string(pager,
		                     "Categories: ",
		                     ui_render_sheet_field);
		if (err)
			return err;
		err = pg_feed_string(pager,
		                     dstr_charp(&thiz->sht_cats),
		                     ui_render_sheet_value);
		if (err)
			return err;
	}

	err = pg_feed_line(pager, 0);
	if (err)
		return err;
	err = pg_feed_string(pager, "Calendar: ", ui_render_sheet_field);
	if (err)
		return err;
	err = pg_feed_string(pager, thiz->sht_cal, ui_render_sheet_value);
	if (err)
		return err;

	if (thiz->sht_loc) {
		err = pg_feed_line(pager, 0);
		if (err)
			return err;
		err = pg_feed_string(pager,
		                     "Location: ",
		                     ui_render_sheet_field);
		if (err)
			return err;
		err = pg_feed_string(pager, thiz->sht_loc, ui_render_sheet_value);
		if (err)
			return err;
	}

	if (thiz->sht_desc) {
		err = pg_feed_line(pager, 0);
		if (err)
			return err;
		err = pg_feed_line(pager, 0);
		if (err)
			return err;
		err = pg_feed_string(pager,
		                     thiz->sht_desc,
		                     ui_render_sheet_value);
		if (err)
			return err;
	}

	return 0;
}

__nonull(1, 2)
int ui_render_sheet(struct ui_sheet* thiz, struct ui_geometry const* geometry)
{
	struct ui_window* const win = &thiz->sht_win;
	struct pager* const     pg = &thiz->sht_pg;
	int                     err;

	ui_update_window_geometry(win, geometry);

	err = ui_render_sheet_pager(thiz, pg, ui_window_width(win));
	if (err)
		return err;

	ui_erase_window(win);

	return pg_paint(pg, thiz->sht_top, geometry->h, win);
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

	pg_clear(&thiz->sht_pg);
}

__nonull(1, 2)
int ui_init_sheet(struct ui_sheet*  thiz,
                  struct todo_repo* todos)
{
	ut_assert(thiz);
	ut_assert(todos);

	int err;

	err = pg_init(&thiz->sht_pg);
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
	pg_fini(&thiz->sht_pg);

	return err;
}

__nonull(1)
void ui_fini_sheet(struct ui_sheet* thiz)
{
	ui_assert_sheet(thiz);

	ui_fini_window(&thiz->sht_win);
	dstr_fini(&thiz->sht_cats);
	pg_fini(&thiz->sht_pg);
}
