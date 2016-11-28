#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>

WINDOW *pad;

static int mrow, mcol;

static char const* const content[] = {
	"1 line",
	"2 line",
	"3 line with...\ncarriage return",
	"4 another line",
	"5 a very loooooooooooooooooooooooooooong looooooooooooooooooooong loooooooooooooooooooooooooooooooooooong line also...\nwith a carriage return.",
	"6 another line",
	"7 another line",
	"8 another line",
	"9 another line",
	"10 another line",
	"11 another line",
	"12 another line",
	"13 another line",
	"14 another line",
	"15 another line",
	"16 another line",
	"17 another line",
	"18 another line",
	"19 another line",
	"20 another line",
	"21 another line",
	"22 another line",
	"23 another line",
	"24 the last one !"
};

int main(void)
{
	unsigned int l;
	int          pos = 0;
	int          c;
	int          nr;

	initscr();
	clear();
	cbreak();
	curs_set(0);

	getmaxyx(stdscr, mrow, mcol);

	pad = newpad(30, mcol);

	for (l = 0; l < (sizeof(content) / sizeof(*content)); l++)
		wprintw(pad, "%s\n", content[l]);

	nr = getcury(pad);

	refresh();
	prefresh(pad, 0, 0, 0, 0, mrow, mcol);

	while((c = wgetch(pad)) != 'q')
	{
		switch (c) {
		case 'k':
			if (pos > 0)
				pos--;
			
			prefresh(pad, pos, 0, 0, 0, mrow, mcol);
			break;

		case 'j':
			if (pos < (nr - 1))
				pos++;

			prefresh(pad, pos, 0, 0, 0, mrow, mcol);
			break;
		}
	}

	delwin(pad);
	clear();
	endwin();

	printf("rows=%d, cols=%d, nr=%d\n", mrow, mcol, nr);
	return 0;
}
