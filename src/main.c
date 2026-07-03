#include "snake.h"

int main()
{	
	pthread_t gui, key;
	setlocale(LC_ALL, "");
	initscr();					/* Start curses mode 		  */
	raw();						/* Line buffering disabled	*/
	keypad(stdscr, TRUE);		/* We get F1, F2 etc..		*/
	noecho();					/* Don't echo() while we do getch */
	curs_set(0);

	if(pthread_create( &gui, NULL, guiLoop, (void *) NULL) != 0){
		perror("reader thread creation failed:");
	}

	if(pthread_create( &key, NULL, keyLoop, (void *) NULL) != 0){
		perror("reader thread creation failed:");
	}

	pthread_join(gui,NULL);
	pthread_cancel(key);
	pthread_join(key, NULL);
	endwin();					/* End curses mode		  */

	return 0;
}
