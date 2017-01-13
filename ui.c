#include <errno.h>
#include "ui.h"

__leaf
int ui_init(void)
{
	/*
	 * Instantiate ncurses.
	 * initscr() exits with an error message in case of error.
	 */
	initscr();

	/* Disable line buffering and erase/kill character-processing. */
	if (cbreak() != OK)
		goto end;

	/* Do not echo typed characters. */
	if (noecho() != OK)
		goto end;

	/* Make cursor invisible. */
	if (curs_set(0) == ERR)
		goto end;

	/* Enable support for special function keys (arrows, etc...). */
	if (keypad(stdscr, TRUE) != OK)
		goto end;

	return 0;

end:
	/* Will implicitly restore initial terminal state. */
	endwin();

	return -EPERM;
}

__leaf
void ui_fini(void)
{
	endwin();
}

/******************************************************************************
 * Window handling.
 ******************************************************************************/

__nonull(1, 2) __printf(2, 3)
void ui_print_window(struct ui_window const* thiz, char const* format, ...)
{
	va_list args;

	va_start(args, format);
	vwprintw(thiz->win_inst, format, args);
	va_end(args);
}

__nonull(1)
void ui_clear_line(struct ui_window const* thiz, unsigned int line_no)
{
	ui_assert_window(thiz);

	ui_move_window_cursor(thiz, line_no, 0);

	wclrtoeol(thiz->win_inst);
}

__nonull(1, 2) __nothrow __leaf __pure
bool ui_window_geometry_changed(struct ui_window const*   thiz,
                                struct ui_geometry const* geometry)
{
	int y, x, h, w;

	getbegyx(thiz->win_inst, y, x);
	getmaxyx(thiz->win_inst, h, w);

	if (((unsigned)y != geometry->y) ||
	    ((unsigned)x != geometry->x) ||
	    ((unsigned)h != geometry->h) ||
	    ((unsigned)w != geometry->w))
		return true;

	return false;
}

__nonull(1, 2) __leaf
void ui_update_window_geometry(struct ui_window const*   thiz,
                               struct ui_geometry const* geometry)
{
	ui_assert_window(thiz);
	ut_assert(geometry);

#ifndef NDEBUG
	int h, w;

	getmaxyx(stdscr, h, w);

	ut_assert((geometry->y + geometry->h) <= (unsigned int)h);
	ut_assert((geometry->x + geometry->w) <= (unsigned int)w);
#endif

	wresize(thiz->win_inst, geometry->h, geometry->w);
	mvwin(thiz->win_inst, geometry->y, geometry->x);
}

__nonull(1, 2) __leaf
int ui_init_window(struct ui_window*         thiz,
                   struct ui_geometry const* geometry)
{
	ut_assert(thiz);

	thiz->win_inst = newwin((int)geometry->h,
	                        (int)geometry->w,
	                        (int)geometry->y,
	                        (int)geometry->x);
	if (!thiz->win_inst)
		return -EPERM;

	return 0;
}

__nonull(1) __leaf
int ui_init_full_window(struct ui_window* thiz)
{
	ut_assert(thiz);

	thiz->win_inst = newwin(0, 0, 0, 0);
	if (!thiz->win_inst)
		return -EPERM;

	return 0;
}

/******************************************************************************
 * Field handling.
 ******************************************************************************/

/*
 * Adjust fields rendering width.
 */
__nonull(1) __nothrow
void ui_adjust_available_field_width(struct ui_field* fields,
                                     unsigned int     row_width,
                                     size_t           field_count)
{
	ut_assert(fields);
	ut_assert(field_count);

	struct ui_field* fld;
	unsigned int     c;
	unsigned int     min;
	unsigned int     tmp = 0;
	unsigned int     fix = 0;
	unsigned int     var = 0;

	/* Probe visible fields. */
	for (c = 0; c < field_count; c++) {
		fld = &fields[c];
		min = fld->fld_min_width;

		ut_assert(min);

		if ((tmp + min) > row_width)
			/* No more space to render remaining fields. */
			break;

		if (fld->fld_fix)
			/* Account visible fixed field widths. */
			fix += min;
		else
			/* Account visible variable field widths. */
			var += fld->fld_optim_width;

		tmp += min;
	}

	/* Mark remaining fields as not visible. */
	while (field_count > c)
		fields[--field_count].fld_avail_width = 0;

	if (!field_count)
		/* Not enough space to render a single field : just return. */
		return;

	/*
	 * Now, for each visible variable field, compute width available for
	 * rendering.
	 */
	row_width -= fix;
	tmp = row_width;
	c = field_count;
	while (c) {
		fld = &fields[--c];

		if (!fld->fld_fix) {
			fld->fld_avail_width = (fld->fld_optim_width *
			                        row_width) / var,
			fld->fld_avail_width = ut_max(fld->fld_avail_width,
			                              fld->fld_min_width);
			fld->fld_avail_width = ut_min(fld->fld_avail_width,
			                              tmp);
			ut_assert(tmp >= fld->fld_avail_width);
			tmp -= fld->fld_avail_width;
		}
		else
			fld->fld_avail_width = fld->fld_min_width;
	}

	/* Evenly spread remaning space over visible fields. */
	row_width = tmp;
	tmp = (row_width + field_count - 1) / field_count;
	for (c = 0; c < field_count; c++) {
		if (!row_width)
			break;

		fld = &fields[c];

		min = ut_min(tmp, row_width);
		fld->fld_avail_width += min;

		row_width -= min;
	}

	ut_assert(!row_width);
}

__nonull(1, 2, 3) __leaf
void ui_render_string_field(struct ui_field const*  thiz,
                            struct ui_window const* window,
                            char const*             string)
{
	ui_assert_field(thiz);
	ut_assert(window);
	ut_assert(string);

	unsigned int const avail = thiz->fld_avail_width;
	unsigned int       tmp;

	/*
	 * There is not enought space to render the minimum number of characters
	 * required by cell / column content : skip it.
	 */
	if (!avail)
		return;

	/* Render cell left margin using spaces. */
	tmp = UI_MARGIN;
	while (tmp--)
		waddch(window->win_inst, ' ');

	/* Render cell content using no more than max_width characters. */
	if (thiz->fld_optim_width && string && (string[0] != '\0')) {
		tmp = getcurx(window->win_inst) - UI_MARGIN;
		waddnstr(window->win_inst, string, avail - (2 * UI_MARGIN));
		tmp = avail - (getcurx(window->win_inst) - tmp);
	}
	else
		tmp = avail - UI_MARGIN;

	/* Padd cell content and add right margin using spaces. */
	while (tmp--)
		waddch(window->win_inst, ' ');
}

__nonull(1, 2, 3) __printf(3, 4)
void ui_render_field(struct ui_field*        thiz,
                     struct ui_window const* window,
                     char const*             format,
                     ...)
{
	ui_assert_field(thiz);
	ut_assert(format);

	unsigned int const avail = thiz->fld_avail_width;
	va_list            args;
	int                ret;

	/*
	 * There is not enought space to render the minimum number of characters
	 * required by cell / column content : skip it.
	 */
	if (!avail)
		return;

	if (thiz->fld_sz < (avail + 1)) {
		free(thiz->fld_buf);
		thiz->fld_sz = 0;
	}

	if (!thiz->fld_sz) {
		thiz->fld_buf = malloc(avail + 1);
		if (!thiz->fld_buf)
			goto err;

		thiz->fld_sz = avail + 1;
	}

	va_start(args, format);
	ret = vsnprintf(thiz->fld_buf, avail + 1, format, args);
	va_end(args);

	if (ret < 0)
		goto err;

	ui_render_string_field(thiz, window, thiz->fld_buf);
	return;

err:
	ui_render_string_field(thiz, window, "???");
}
