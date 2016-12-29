#ifndef _UI_CONSOLE_H
#define _UI_CONSOLE_H

#include "ui.h"

#define UI_CONSOLE_PROMPT_CHAR ':'

struct ui_console {
	struct ui_window      cons_win;
	char const*           cons_str;
	size_t                cons_len;
};

#define ui_assert_console(_cons) \
	ut_assert(_cons);        \

static inline __nonull(1)
void ui_show_console(struct ui_console const* thiz)
{
	ui_assert_console(thiz);

	ui_show_window(&thiz->cons_win);
}

extern void        ui_render_console(struct ui_console const*,
                                     struct ui_geometry const*)
                   __nonull(1, 2);
extern int         ui_process_console_input(struct ui_console const*,
                                            char const**)
                   __nonull(1, 2);
extern void        ui_enable_console_input(struct ui_console*)
                   __nonull(1);
extern void        ui_post_console_message(struct ui_console*, char const*)
                   __nonull(1, 2) __leaf;
extern int         ui_init_console(struct ui_console*)
                   __nonull(1) __leaf;
extern void        ui_fini_console(struct ui_console const*)
                   __nonull(1) ;

#endif
