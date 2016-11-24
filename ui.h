#ifndef _UI_H
#define _UI_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ncurses.h>
#include "utils.h"

#define UI_MARGIN (1)

static inline
void ui_show(void)
{
	wrefresh(stdscr);
}

extern int  ui_init(void) __leaf;
extern void ui_fini(void) __leaf;

/******************************************************************************
 * Window handling.
 ******************************************************************************/

struct ui_geometry {
	unsigned int y;
	unsigned int x;
	unsigned int h;
	unsigned int w;
};

struct ui_window {
	WINDOW* win_inst;
};

#define ui_assert_window(_win)   \
	ut_assert(_win);         \
	ut_assert(_win->win_inst)

static inline __nonull(1)
unsigned int ui_window_width(struct ui_window const* thiz)
{
	ui_assert_window(thiz);

	return getmaxx(thiz->win_inst);
}

static inline __nonull(1)
unsigned int ui_window_height(struct ui_window const* thiz)
{
	ui_assert_window(thiz);

	return getmaxy(thiz->win_inst);
}

static inline __nonull(1)
void ui_fetch_window_attrs(struct ui_window const* thiz,
                           attr_t*                 attributes,
                           short*                  color)
{
	ui_assert_window(thiz);

	wattr_get(thiz->win_inst, attributes, color, NULL);
}

static inline __nonull(1)
void ui_setup_window_attrs(struct ui_window const* thiz,
                           attr_t                  attributes,
                           short                   color)
{
	ui_assert_window(thiz);

	wattr_set(thiz->win_inst, attributes, color, NULL);
}

static inline __nonull(1)
void ui_setup_line_attrs(struct ui_window const* thiz,
                         unsigned int            line,
                         attr_t                  attributes,
                         short                   color)
{
	ui_assert_window(thiz);

	mvwchgat(thiz->win_inst, line, 0, -1, attributes, color, NULL);
}

static inline __nonull(1)
void ui_setup_current_line_attrs(struct ui_window const* thiz,
                                 attr_t                  attributes,
                                 short                   color)
{
	ui_assert_window(thiz);

	wchgat(thiz->win_inst, -1, attributes, color, NULL);
}

static inline __nonull(1)
void ui_window_attron(struct ui_window const* thiz, int attributes)
{
	ui_assert_window(thiz);

	wattron(thiz->win_inst, attributes);
}

static inline __nonull(1)
void ui_window_attroff(struct ui_window const* thiz, int attributes)
{
	ui_assert_window(thiz);

	wattroff(thiz->win_inst, attributes);
}

static inline __nonull(1)
void ui_move_window_cursor(struct ui_window const* thiz, int y, int x)
{
	ui_assert_window(thiz);

	wmove(thiz->win_inst, y, x);
}

static inline __nonull(1)
void ui_erase_window(struct ui_window const* thiz)
{
	ui_assert_window(thiz);

	werase(thiz->win_inst);
}

static inline
void ui_print_window_string(struct ui_window const* thiz,
                            char const*             string)
{
	ui_assert_window(thiz);

	waddstr(thiz->win_inst, string);
}

static inline __nonull(1)
void ui_show_window(struct ui_window const* thiz)
{
	ui_assert_window(thiz);

	wrefresh(thiz->win_inst);
}

static inline __nonull(1)
void ui_fini_window(struct ui_window const* thiz)
{
	ui_assert_window(thiz);

	delwin(thiz->win_inst);
}

extern bool ui_window_geometry_changed(struct ui_window const*,
                                       struct ui_geometry const*)
            __nonull(1, 2) __nothrow __leaf __pure;

extern void ui_update_window_geometry(struct ui_window const*,
                                      struct ui_geometry const*)
            __nonull(1, 2) __leaf;

extern int  ui_init_window(struct ui_window*,
                           struct ui_geometry const*) __nonull(1, 2) __leaf;

extern int  ui_init_full_window(struct ui_window*) __nonull(1) __leaf;

/******************************************************************************
 * Field handling.
 ******************************************************************************/

/*
 * Field descriptor.
 * All widths documented here include margins and are expressed as number of
 * underlying window columns.
 */
struct ui_field {
	/* Indicate a fix or variable field width. */
	bool         fld_fix;
	/* Minimum width required to properly render this field. */
	unsigned int fld_min_width;
	/* Optimal width allowing to render full field content. */
	unsigned int fld_optim_width;
	/* Width available for rendering this field. */
	unsigned int fld_avail_width;
	/* Temporary buffer used to render printf-like strings. */
	char*        fld_buf;
	/* Size of above temporary buffer. */
	size_t       fld_sz;
};

#define ui_assert_field(_field)                       \
	ut_assert(_field);                            \
	ut_assert(!(!!_field->fld_buf ^ !!_field->fld_sz))

static inline __nonull(1) __nothrow
void ui_adjust_optimal_field_width(struct ui_field* thiz, unsigned int len)
{
	ui_assert_field(thiz);

	if (len)
		len += 2 * UI_MARGIN;

	thiz->fld_optim_width = ut_max(thiz->fld_optim_width, len);
}

static inline __nonull(1) __nothrow
void ui_reset_optimal_field_width(struct ui_field* thiz, unsigned int len)
{
	ui_assert_field(thiz);

	if (len)
		len += 2 * UI_MARGIN;

	thiz->fld_optim_width = len;
}

static inline __nonull(1) __nothrow
void ui_init_fixed_field(struct ui_field* thiz, unsigned int width)
{
	thiz->fld_fix = true;
	thiz->fld_min_width = width + (2 * UI_MARGIN);
	thiz->fld_optim_width = width + (2 * UI_MARGIN);
	thiz->fld_avail_width = UINT_MAX;
	thiz->fld_buf = NULL;
	thiz->fld_sz = 0;
}

static inline __nonull(1) __nothrow
void ui_init_variable_field(struct ui_field* thiz, unsigned int min_width)
{
	thiz->fld_fix = false;
	thiz->fld_min_width = min_width + (2 * UI_MARGIN);
	thiz->fld_optim_width = 0;
	thiz->fld_avail_width = UINT_MAX;
	thiz->fld_buf = NULL;
	thiz->fld_sz = 0;
}

static inline __nonull(1) __nothrow
void ui_fini_field(struct ui_field const* thiz)
{
	ui_assert_field(thiz);

	if (thiz->fld_sz)
		free(thiz->fld_buf);
}

extern void ui_adjust_available_field_width(struct ui_field*,
                                            unsigned int,
                                            size_t) __nonull(1) __nothrow;

extern void ui_render_string_field(struct ui_field const*,
                                   struct ui_window const*,
                                   char const*) __nonull(1, 2, 3) __leaf;
extern void ui_render_field(struct ui_field* thiz,
                            struct ui_window const*,
                            char const*,
                            ...) __nonull(1, 2, 3) __printf(3, 4);

/******************************************************************************
 * Scrolling list handling.
 ******************************************************************************/

struct ui_scroll_list {
	unsigned int scrl_top;
	unsigned int scrl_rows;
	unsigned int scrl_cols;
};

static inline __nonull(1) __nothrow
void ui_reset_scroll_list(struct ui_scroll_list* thiz)
{
	ut_assert(thiz);

	memset(thiz, 0, sizeof(*thiz));
}

static inline __nonull(1, 2) __nothrow __pure
bool ui_scroll_list_width_changed(struct ui_scroll_list const* thiz,
                                  struct ui_geometry const*    geometry)
{
	return geometry->w != thiz->scrl_cols;
}

struct ui_scroll_list_refresh {
	unsigned int scrl_top;
	unsigned int scrl_row;
	unsigned int scrl_cnt;
};

extern unsigned int ui_render_scroll_list(struct ui_scroll_list*,
                                          unsigned int,
                                          unsigned int,
                                          struct ui_geometry const*,
                                          struct ui_scroll_list_refresh*)
                    __nonull(1, 4, 5) __nothrow __leaf;

#endif
