#include "todo.h"
#include "ui_todo.h"
#include "ui_sheet.h"

#define SHT_INIT_CAT_LEN (8U)

__nonull(1, 2)
void ui_render_sheet(struct ui_sheet*          thiz,
                     struct ui_geometry const* geometry,
                     unsigned int              index)
{
	ui_assert_sheet(thiz);
	ut_assert(index < todo_repo_count(thiz->sht_todos));

	struct ui_window* const win = &thiz->sht_win;
	bool                    load = (index != thiz->sht_index);
	bool                    resize;
	int                     ln = UI_MARGIN;

	resize = ui_window_geometry_changed(win, geometry);
	ut_assert(load || resize);

	if (load) {
		struct todo* todo;

		todo = todo_repo_entry(thiz->sht_todos, index);

		thiz->sht_tle = ui_todo_title(todo);

		ui_todo_priority(todo,
		                 thiz->sht_prio,
		                 sizeof(thiz->sht_prio));

		ui_todo_categories(todo, &thiz->sht_cats);

		thiz->sht_cal = ui_todo_calendar(todo);

		thiz->sht_index = index;
	}

	if (resize)
		ui_update_window_geometry(win, geometry);

	werase(win->win_inst);
	mvwprintw(win->win_inst, ln++, UI_MARGIN, "Title: %s", thiz->sht_tle);
	mvwprintw(win->win_inst, ln++, UI_MARGIN, "Priority: %s", thiz->sht_prio);
	mvwprintw(win->win_inst, ln++, UI_MARGIN, "Categories: %s", dstr_charp(&thiz->sht_cats));
	mvwprintw(win->win_inst, ln, UI_MARGIN, "Calendar: %s", thiz->sht_cal);
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
