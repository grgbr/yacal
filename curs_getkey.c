#include <stdlib.h>
#include <ncurses.h>

int main(void)
{
#if 0
    int lastch = ERR;
#endif
    int ch;
    WINDOW *win;

    initscr();
    (void) cbreak();		/* take input chars one at a time, no wait for \n */
    (void) noecho();		/* don't echo input */

    printw("Type any key to display informations about it (^C to quit):");
    refresh();

    win = newwin(LINES - 2, COLS, 2, 0);
    scrollok(win, TRUE);
    keypad(win, TRUE);
    wmove(win, 0, 0);

    while ((ch = wgetch(win)) != ERR) {
	const char *name = keyname(ch);
	char *bind = keybound(ch, 0);
	wprintw(win, "Keycode %d, name %s, binding \"%s\"\n",
		ch,
		name != 0 ? name : "<null>",
		bind != 0 ? bind : "<null>");
	free(bind);
	wclrtoeol(win);
	wrefresh(win);
#if 0
	if (ch >= KEY_MIN) {
	    keyok(ch, FALSE);
	    lastch = ch;
	} else if (lastch >= KEY_MIN) {
	    keyok(lastch, TRUE);
	}
#endif
    }
    endwin();

    return EXIT_SUCCESS;
}

