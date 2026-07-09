#include "snake.h"

int main()
{	
	setlocale(LC_ALL, "");
	initscr();					/* Start curses mode 		  */
	raw();						/* Line buffering disabled	*/
	keypad(stdscr, TRUE);		/* We get F1, F2 etc..		*/
	noecho();					/* Don't echo() while we do getch */
	curs_set(0);
	nodelay(stdscr, TRUE);		/* getch() becomes non-blocking */

	guiLoop();

	endwin();					/* End curses mode		  */

	return 0;
}
