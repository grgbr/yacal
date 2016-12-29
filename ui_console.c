/* WORK IN PROGRESS */
/* See https://github.com/ulfalizer/readline-and-ncurses/blob/master/rlncurses.c */
/* https://cnswww.cns.cwru.edu/php/chet/readline/readline.html#IDX240 */

#include <errno.h>
#include <readline/history.h>
#include <readline/readline.h>
#include "string.h"
#include "ui_console.h"

static int               rdln_inst;
static char              rdln_char;
static char*             rdln_ln;
static char const*       rdln_cmd;
static struct ui_window* rdln_win;

static
void rdln_process_line(char* line)
{
	char* cmd;

	cmd = str_strip_blank(line);

	if (cmd && (*cmd != '\0'))
		add_history(cmd);

	if (rdln_ln) {
		free(rdln_ln);
		rdln_ln = line;
	}

	ut_assert(!rdln_cmd);

	rdln_cmd = cmd;
}

static
int rdln_getc(FILE* unused __unused)
{
	return rdln_char;
}

static
void rdln_ungetc(int c)
{
	rdln_char = c;
	rl_callback_read_char();
}

char const* rdln_parse(int c)
{
	rdln_ungetc(c);

	if (rdln_cmd) {
		char const* cmd = rdln_cmd;

		rdln_cmd = NULL;
		return cmd;
	}

	return NULL;
}

/*
 * Readline will call this when it feels the need to update display to reflect
 * editing buffer's current content.
 */
static
void rdln_redisplay(void)
{
	/* Clear command line window. */
	ui_erase_window(rdln_win);
	ui_move_window_cursor(rdln_win, 0, 0);

	/* Print content of readline's editing buffer prefixed with prompt. */
	ui_print_window_char(rdln_win, UI_CONSOLE_PROMPT_CHAR);
	if (rl_end)
		ui_print_window_string(rdln_win, rl_line_buffer, rl_end);

	/*
	 * Update window cursor to point to current readline cursor position.
	 * This allows to reflect user / readline interactions, i.e. move
	 * forward, backward, backspace, etc...
	 */
	ui_move_window_cursor(rdln_win, 0, rl_point + 1);
}

static
void rdln_init(struct ui_window* window)
{
	if (rdln_inst++)
		return;

	rdln_win = window;

	/* Allow conditional parsing of the ~/.inputrc file. */
	rl_readline_name = "yacal";

	/*
	 * Prevent readline from (de)initializing terminal (leave this to
	 * ncurses).
	 */
	rl_prep_term_function = NULL;
	rl_deprep_term_function = NULL;

	/*
	 * Prevent readline from messing with ncurses and our application signal
	 * handling.
	 */
	rl_catch_signals = 0;

	/* Prevent readline from installing its own SIGWINCH handler. */
	rl_catch_sigwinch = 0;

	/*
	 * Prevent readline from modifying LINES and COLUMNS environment
	 * variables (already set by ncurses).
	 */
	rl_change_environment = 0;

	/*
	 * Instruct readline to trust LINES and COLUMNS environment variables
	 * instead of fetching screen dimensions from kernel. This allows proper
	 * interactions with ncurses.
	 */
	rl_prefer_env_winsize = 1;

	/* Tell the completer that we want a crack first. */
	//rl_attempted_completion_function = fileman_completion;

	/* Feed characters manually to readline. */
	rl_getc_function = rdln_getc;
	rl_redisplay_function = rdln_redisplay;

	/* Install readline line processor. */
	static char const prompt[] = { UI_CONSOLE_PROMPT_CHAR, '\0' };
	rl_callback_handler_install(prompt, rdln_process_line);

	rl_variable_bind("horizontal-scroll-mode", "On");
}

static
void rdln_fini()
{
	ut_assert(rdln_inst > 0);

	if (--rdln_inst)
		return;

	rl_callback_handler_remove();

	if (rdln_ln)
		free(rdln_ln);
}

__nonull(1, 2)
void ui_render_console(struct ui_console const*  thiz,
                       struct ui_geometry const* geometry)
{
	ui_assert_console(thiz);

	struct ui_window const* const win = &thiz->cons_win;

	ui_update_window_geometry(win, geometry);

	if (!thiz->cons_len)
		return;

	ui_erase_window(win);
	ui_move_window_cursor(win, 0, 0);
	ui_print_window_string(win,
	                       thiz->cons_str,
	                       ut_min(geometry->w, thiz->cons_len));
}

__nonull(1)
void ui_enable_console_input(struct ui_console* thiz)
{
	ui_assert_console(thiz);

	struct ui_window const* const win = &thiz->cons_win;

	ui_erase_window(win);
	ui_move_window_cursor(win, 0, 0);
	ui_print_window_char(win, UI_CONSOLE_PROMPT_CHAR);

	curs_set(1);

	thiz->cons_len = 0;
}

static
void ui_disable_console_input(void)
{
	curs_set(0);
}

__nonull(1, 2)
int ui_process_console_input(struct ui_console const* thiz,
                             char const**             command)
{
	ui_assert_console(thiz);
	ut_assert(!thiz->cons_len);

	int ret;

	ret = ui_fetch_window_char(&thiz->cons_win);
	if ((ret < 0) || (ret == KEY_RESIZE))
		/*
		 * Let upper controller handle window resizing and special
		 * conditions such as EINTR or EAGAIN.
		 */
		return ret;

	switch (ret) {
	case KEY_UP:
		rl_get_previous_history(1, ret);
		break;

	case KEY_DOWN:
		rl_get_next_history(1, ret);
		break;

	case KEY_LEFT:
		rl_backward_char(1, ret);
		break;

	case KEY_RIGHT:
		rl_forward_char(1, ret);
		break;

	case KEY_HOME:
		rl_beg_of_line(0, ret);
		break;

	case KEY_END:
		rl_end_of_line(0, ret);
		break;

	case KEY_BACKSPACE:
		if (!rl_point)
			return -EAGAIN;
		rl_backward_char (1, ret);

	case KEY_DC:
		rl_delete(1, ret);
		break;

	default:
		errno = 0;
		*command = rdln_parse(ret);
		if (!*command) {
			if (!errno)
				return -EAGAIN;
			ret = -errno;
		}
		else
			ret = 0;

		ui_disable_console_input();

		return ret;
	}

	rl_forced_update_display();

	return -EAGAIN;
}

__nonull(1, 2)
void ui_post_console_message(struct ui_console* thiz, char const* message)
{
	ui_assert_console(thiz);
	ut_assert(message);

	/* TODO: limit to max window width. */
	thiz->cons_str = message;
	thiz->cons_len = strlen(message);
}

__nonull(1) __leaf
int ui_init_console(struct ui_console* thiz)
{
	ut_assert(thiz);

	int err;

	/* Create console window. */
	err = ui_init_full_window(&thiz->cons_win);
	if (err)
		return err;

	if (keypad(thiz->cons_win.win_inst, TRUE) != OK) {
		ui_fini_window(&thiz->cons_win);
		return -EPERM;
	}

	thiz->cons_len = 0;

	rdln_init(&thiz->cons_win);

	return 0;
}

__nonull(1)
void ui_fini_console(struct ui_console const* thiz)
{
	ui_assert_console(thiz);

	rdln_fini();

	ui_fini_window(&thiz->cons_win);
}
