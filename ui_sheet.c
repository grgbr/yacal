#include "todo.h"
#include "ui_todo.h"
#include "ui_sheet.h"

#define SHT_INIT_CAT_LEN (8U)

static __nonull(1, 3, 4)
void ui_render_sheet_field(struct ui_window const* window,
                           int                     row,
                           char const*             header,
                           char const*             content)
{
	ui_move_window_cursor(window, row, 0);
	ui_window_attron(window, A_BOLD);
	ui_print_window_string(window, header);
	ui_window_attroff(window, A_BOLD);
	ui_print_window_string(window, content);
}

__nonull(1, 2)
void ui_render_sheet(struct ui_sheet*          thiz,
                     struct ui_geometry const* geometry,
                     unsigned int              index)
{
	ui_assert_sheet(thiz);
	ut_assert(index < todo_repo_count(thiz->sht_todos));

	struct ui_window* const win = &thiz->sht_win;
	bool                    load = (index != thiz->sht_index);
	int                     ln = 0;

	if (load) {
		struct todo* todo;

		todo = todo_repo_entry(thiz->sht_todos, index);

		thiz->sht_tle = ui_todo_title(todo);

		ui_todo_priority(todo,
		                 thiz->sht_prio,
		                 sizeof(thiz->sht_prio));

		ui_todo_categories(todo, &thiz->sht_cats);

		thiz->sht_cal = ui_todo_calendar(todo);

		thiz->sht_loc = ui_todo_location(todo);

		thiz->sht_desc = ui_todo_description(todo);

		thiz->sht_index = index;
	}

	ui_update_window_geometry(win, geometry);

	ui_erase_window(win);

	ui_render_sheet_field(win, ln++, "Title: ", thiz->sht_tle);
	ui_render_sheet_field(win, ln++, "Priority: ", thiz->sht_prio);
	ui_render_sheet_field(win, ln++, "Categories: ", dstr_charp(&thiz->sht_cats));
	ui_render_sheet_field(win, ln++, "Calendar: ", thiz->sht_cal);
	ui_render_sheet_field(win, ln, "Location: ", thiz->sht_loc);

	ui_move_window_cursor(win, ln + 2, 0);
	ui_print_window_string(win, thiz->sht_desc);
}

__nonull(1, 2)
int ui_init_sheet(struct ui_sheet*  thiz,
                  struct todo_repo* todos)
{
	ut_assert(thiz);
	ut_assert(todos);

	int err;

	err = dstr_init(&thiz->sht_cats, SHT_INIT_CAT_LEN);
	if (err)
		return err;

	/* Create window leaving enought place for status bar. */
	err = ui_init_full_window(&thiz->sht_win);
	if (err) {
		dstr_fini(&thiz->sht_cats);
		return err;
	}

	thiz->sht_index = UINT_MAX;
	thiz->sht_todos = todos;

	return 0;
}

__nonull(1)
void ui_fini_sheet(struct ui_sheet const* thiz)
{
	ui_assert_sheet(thiz);

	ui_fini_window(&thiz->sht_win);
}
