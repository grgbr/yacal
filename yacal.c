#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "todo.h"
#include "ui.h"
#include "ui_digest.h"
#include "ui_status.h"
#include "ui_sheet.h"
#include "ui_console.h"

enum yacal_state {
	YACAL_DIGEST_STAT,
	YACAL_FULL_STAT
};

struct yacal {
	enum yacal_state   ycl_state;
	bool               ycl_cmd;
	struct todo_repo   ycl_todos;
	struct ui_geometry ycl_dgst_geom;
	struct ui_geometry ycl_stat_geom;
	struct ui_geometry ycl_sht_geom;
	struct ui_geometry ycl_cons_geom;
	struct ui_digest   ycl_dgst;
	struct ui_status   ycl_stat;
	struct ui_sheet    ycl_sht;
	struct ui_console  ycl_cons;
};

static void ycl_resize_layout(struct yacal* thiz)
{
	int h;
	int w;

	getmaxyx(stdscr, h, w);

	switch (thiz->ycl_state) {
	case YACAL_DIGEST_STAT:
		thiz->ycl_dgst_geom.y = 0;
		thiz->ycl_dgst_geom.x = 0;
		thiz->ycl_dgst_geom.h = h - 2;
		thiz->ycl_dgst_geom.w = w;

		thiz->ycl_stat_geom.y = h - 2;
		thiz->ycl_stat_geom.x = 0;
		thiz->ycl_stat_geom.h = 1;
		thiz->ycl_stat_geom.w = w;

		thiz->ycl_cons_geom.y = h - 1;
		thiz->ycl_cons_geom.x = 0;
		thiz->ycl_cons_geom.h = 1;
		thiz->ycl_cons_geom.w = w;

		break;

	case YACAL_FULL_STAT: {
		int sht, dgst;
		
		dgst = ((h - 2) * 6) / 10;
		sht = h - 2 - dgst;

		thiz->ycl_dgst_geom.y = 0;
		thiz->ycl_dgst_geom.x = 0;
		thiz->ycl_dgst_geom.h = dgst;
		thiz->ycl_dgst_geom.w = w;

		thiz->ycl_stat_geom.y = dgst;
		thiz->ycl_stat_geom.x = 0;
		thiz->ycl_stat_geom.h = 1;
		thiz->ycl_stat_geom.w = w;

		thiz->ycl_sht_geom.y = dgst + 1;
		thiz->ycl_sht_geom.x = 0;
		thiz->ycl_sht_geom.h = sht;
		thiz->ycl_sht_geom.w = w;

		thiz->ycl_cons_geom.y = h - 1;
		thiz->ycl_cons_geom.x = 0;
		thiz->ycl_cons_geom.h = 1;
		thiz->ycl_cons_geom.w = w;

		break;
	}

	default:
		ut_assert(0);
	}
}

static void ycl_resize(struct yacal* thiz)
{
	ycl_resize_layout(thiz);

	ui_render_digest(&thiz->ycl_dgst, &thiz->ycl_dgst_geom);

	ui_render_status(&thiz->ycl_stat, &thiz->ycl_stat_geom);

	if (thiz->ycl_state == YACAL_FULL_STAT)
		ui_render_sheet(&thiz->ycl_sht, &thiz->ycl_sht_geom);

	ui_render_console(&thiz->ycl_cons, &thiz->ycl_cons_geom);
}

static void ycl_select_todo(struct yacal* thiz)
{
	unsigned int select;

	select = ui_digest_entry(&thiz->ycl_dgst);

	ui_select_status(&thiz->ycl_stat, select);
	ui_render_status(&thiz->ycl_stat, &thiz->ycl_stat_geom);

	if (thiz->ycl_state == YACAL_FULL_STAT) {
		ui_load_sheet(&thiz->ycl_sht, select);
		ui_render_sheet(&thiz->ycl_sht, &thiz->ycl_sht_geom);
	}
}

static void ycl_show(struct yacal const* thiz)
{
	ui_show();

	ui_show_digest(&thiz->ycl_dgst);

	ui_show_status(&thiz->ycl_stat);

	if (thiz->ycl_state == YACAL_FULL_STAT)
		ui_show_sheet(&thiz->ycl_sht);

	ui_show_console(&thiz->ycl_cons);
}

static int ycl_process_input(struct yacal* thiz)
{
	int         c;
	char const* cmd;

	if (!thiz->ycl_cmd)
		return getch();

	c = ui_process_console_input(&thiz->ycl_cons, &cmd);
	if (c > 0)
		goto out;

	if (c == -EAGAIN)
		goto render;

	if (c) {
		ui_post_console_message(&thiz->ycl_cons,
		                        "failed to parse command");
		goto disable;
	}

	/* TODO: parse and execute command. */
	ui_post_console_message(&thiz->ycl_cons, cmd);

disable:
	thiz->ycl_cmd = false;
render:
	ui_render_console(&thiz->ycl_cons, &thiz->ycl_cons_geom);
out:
	return c;
}

static int ycl_process(struct yacal* thiz)
{
	int c;

	c = ycl_process_input(thiz);
	if (c <= 0)
		goto show;

	switch (c) {
	case KEY_RESIZE:
		ycl_resize(thiz);
		break;

	case KEY_DOWN:
	case 'j':
		if (ui_scroll_digest_down(&thiz->ycl_dgst))
			ycl_select_todo(thiz);
		break;

	case KEY_UP:
	case 'k':
		if(ui_scroll_digest_up(&thiz->ycl_dgst))
			ycl_select_todo(thiz);
		break;

	case KEY_ENTER:
	case '\n':
		switch (thiz->ycl_state) {
		case YACAL_DIGEST_STAT:
			thiz->ycl_state = YACAL_FULL_STAT;
			ui_load_sheet(&thiz->ycl_sht,
			              ui_digest_entry(&thiz->ycl_dgst));
			ycl_resize(thiz);
			break;

		case YACAL_FULL_STAT:
			ui_scroll_sheet_down(&thiz->ycl_sht);
			break;

		default:
			ut_assert(0);
		}

		break;

	case KEY_BACKSPACE:
		if (thiz->ycl_state == YACAL_FULL_STAT)
			ui_scroll_sheet_up(&thiz->ycl_sht);

		break;

	case 'q':
		switch (thiz->ycl_state) {
		case YACAL_DIGEST_STAT:
			return 1;

		case YACAL_FULL_STAT:
			thiz->ycl_state = YACAL_DIGEST_STAT;
			ycl_resize(thiz);
			break;

		default:
			ut_assert(0);
		}

		break;

	case UI_CONSOLE_PROMPT_CHAR:
		thiz->ycl_cmd = true;
		ui_enable_console_input(&thiz->ycl_cons);
		break;

	default:
		return 0;
	}

show:
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

	err = ui_init_digest(&thiz->ycl_dgst, &thiz->ycl_todos);
	if (err)
		goto fini_ui;

	err = ui_init_status(&thiz->ycl_stat, &thiz->ycl_todos);
	if (err)
		goto fini_dgst;

	err = ui_init_sheet(&thiz->ycl_sht, &thiz->ycl_todos);
	if (err)
		goto fini_stat;

	err = ui_init_console(&thiz->ycl_cons);
	if (err)
		goto fini_sht;

	err = todo_load_repo(&thiz->ycl_todos);
	if (err)
		goto fini_cons;

	thiz->ycl_state = YACAL_DIGEST_STAT;
	thiz->ycl_cmd = false;
	ycl_resize_layout(thiz);

	ui_load_digest(&thiz->ycl_dgst);
	ui_load_status(&thiz->ycl_stat, 0);

	ui_render_digest(&thiz->ycl_dgst, &thiz->ycl_dgst_geom);
	ui_render_status(&thiz->ycl_stat, &thiz->ycl_stat_geom);

	ui_post_console_message(&thiz->ycl_cons, "Welcome to Yacal");
	ui_render_console(&thiz->ycl_cons, &thiz->ycl_cons_geom);

	ycl_show(thiz);

	return 0;

fini_cons:
	ui_fini_console(&thiz->ycl_cons);
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
	ui_fini_console(&thiz->ycl_cons);
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
