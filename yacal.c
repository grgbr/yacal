#include <stdio.h>
#include <string.h>
#include "todo.h"
#include "ui.h"
#include "ui_digest.h"
#include "ui_status.h"
#include "ui_sheet.h"

enum yacal_state {
	YACAL_DIGEST_STAT,
	YACAL_FULL_STAT
};

struct yacal {
	enum yacal_state   ycl_state;
	unsigned int       ycl_curr_todo;
	struct todo_repo   ycl_todos;
	struct ui_geometry ycl_dgst_geom;
	struct ui_geometry ycl_stat_geom;
	struct ui_geometry ycl_sht_geom;
	struct ui_digest   ycl_dgst;
	struct ui_status   ycl_stat;
	struct ui_sheet    ycl_sht;
};

static void ycl_resize_layout(struct yacal* thiz)
{
	int h;
	int w;
	int sht = 10;

	getmaxyx(stdscr, h, w);

	switch (thiz->ycl_state) {
	case YACAL_DIGEST_STAT:
		thiz->ycl_dgst_geom.y = 0;
		thiz->ycl_dgst_geom.x = 0;
		thiz->ycl_dgst_geom.h = h - 1;
		thiz->ycl_dgst_geom.w = w;

		thiz->ycl_stat_geom.y = h - 1;
		thiz->ycl_stat_geom.x = 0;
		thiz->ycl_stat_geom.h = 1;
		thiz->ycl_stat_geom.w = w;

		break;

	case YACAL_FULL_STAT:
		thiz->ycl_dgst_geom.y = 0;
		thiz->ycl_dgst_geom.x = 0;
		thiz->ycl_dgst_geom.h = h - 1 - sht;
		thiz->ycl_dgst_geom.w = w;

		thiz->ycl_stat_geom.y = h - 1 - sht;
		thiz->ycl_stat_geom.x = 0;
		thiz->ycl_stat_geom.h = 1;
		thiz->ycl_stat_geom.w = w;

		thiz->ycl_sht_geom.y = h - sht;
		thiz->ycl_sht_geom.x = 0;
		thiz->ycl_sht_geom.h = sht;
		thiz->ycl_sht_geom.w = w;

		break;

	default:
		ut_assert(0);
	}
}

static void ycl_render(struct yacal* thiz, unsigned int index)
{
	ui_render_digest(&thiz->ycl_dgst, &thiz->ycl_dgst_geom, index);
	ui_render_status(&thiz->ycl_stat, &thiz->ycl_stat_geom, index);

	if (thiz->ycl_state == YACAL_FULL_STAT)
		ui_render_sheet(&thiz->ycl_sht, &thiz->ycl_sht_geom, index);

	thiz->ycl_curr_todo = index;
}

static void ycl_show(struct yacal const* thiz)
{
	ui_show();

	ui_show_digest(&thiz->ycl_dgst);

	ui_show_status(&thiz->ycl_stat);

	if (thiz->ycl_state == YACAL_FULL_STAT)
		ui_show_sheet(&thiz->ycl_sht);
}

static void ycl_select_next_todo(struct yacal* thiz)
{
	unsigned int const idx = thiz->ycl_curr_todo + 1;

	if (idx >= todo_repo_count(&thiz->ycl_todos))
		return;

	ycl_render(thiz, idx);
}

static void ycl_select_previous_todo(struct yacal* thiz)
{
	unsigned int const idx = thiz->ycl_curr_todo;

	if (!idx)
		return;

	ycl_render(thiz, idx - 1);
}

static int ycl_process(struct yacal* thiz)
{
	switch (getch()) {
	case KEY_RESIZE:
		ycl_resize_layout(thiz);
		ycl_render(thiz, thiz->ycl_curr_todo);
		break;

	case KEY_DOWN:
	case 'j':
		ycl_select_next_todo(thiz);
		break;

	case KEY_UP:
	case 'k':
		ycl_select_previous_todo(thiz);
		break;

	case KEY_ENTER:
	case '\n':
		switch (thiz->ycl_state) {
		case YACAL_DIGEST_STAT:
			thiz->ycl_state = YACAL_FULL_STAT;
			ycl_resize_layout(thiz);
			break;

		case YACAL_FULL_STAT:
			break;

		default:
			ut_assert(0);
		}

		ycl_render(thiz, thiz->ycl_curr_todo);
		break;

	case 'q':
		switch (thiz->ycl_state) {
		case YACAL_DIGEST_STAT:
			return 1;

		case YACAL_FULL_STAT:
			thiz->ycl_state = YACAL_DIGEST_STAT;
			ycl_resize_layout(thiz);
			break;

		default:
			ut_assert(0);
		}

		ycl_render(thiz, thiz->ycl_curr_todo);
		break;

	default:
		return 0;
	}

	ycl_show(thiz);
	return 0;
}

static
int ycl_init(struct yacal* thiz)
{
	int err;

	err = todo_init_repo(&thiz->ycl_todos,
	                     "Kether",
	                     "testdata",
	                     todo_sortby_summary);
	if (err)
		return err;

	err = ui_init();
	if (err)
		goto fini_todo;

	thiz->ycl_state = YACAL_DIGEST_STAT;
	ycl_resize_layout(thiz);

	err = ui_init_digest(&thiz->ycl_dgst,
	                     &thiz->ycl_dgst_geom,
	                     &thiz->ycl_todos);
	if (err)
		goto fini_ui;

	err = ui_init_status(&thiz->ycl_stat,
	                     &thiz->ycl_stat_geom,
	                     &thiz->ycl_todos);
	if (err)
		goto fini_dgst;

	err = ui_init_sheet(&thiz->ycl_sht, &thiz->ycl_todos);
	if (err)
		goto fini_stat;

	err = todo_load_repo(&thiz->ycl_todos);
	if (err)
		goto fini_sht;

	ui_load_digest(&thiz->ycl_dgst);
	ui_load_status(&thiz->ycl_stat);

	ycl_render(thiz, 0);

	ycl_show(thiz);

	return 0;

fini_sht:
	ui_fini_sheet(&thiz->ycl_sht);
fini_stat:
	ui_fini_status(&thiz->ycl_stat);
fini_dgst:
	ui_fini_digest(&thiz->ycl_dgst);
fini_ui:
	ui_fini();
fini_todo:
	todo_fini_repo(&thiz->ycl_todos);

	return err;
}

static void ycl_fini(struct yacal* thiz)
{
	ui_fini_sheet(&thiz->ycl_sht);
	ui_fini_status(&thiz->ycl_stat);
	ui_fini_digest(&thiz->ycl_dgst);
	ui_fini();
	todo_fini_repo(&thiz->ycl_todos);
}

/* TODO: handle errors through status bar (if no database ...) ? */
int main(/*int argc, const char* argv[]*/)
{
	struct yacal ctrl;
	int          err;
	int          ret = EXIT_FAILURE;

	err = ycl_init(&ctrl);
	if (err)
		return ret;

	do {
		err = ycl_process(&ctrl);
	} while (!err);

	if (err >= 0)
		ret = EXIT_SUCCESS;

	ycl_fini(&ctrl);

	return ret;
}
