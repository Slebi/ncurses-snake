#include <ncurses.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#define KEY_ESCAPE 			27 
#define REDRAW_INTERVAL		100
#define FOOD_INTERVAL		20

typedef struct snake_part_struct {
	struct snake_part_struct* next;
	struct snake_part_struct* prev;
	int x;
	int y;
} snake_part;

typedef struct snake_struct {
	snake_part* head;
	snake_part* butt;
} snake;

typedef struct food_struct {
	struct food_struct* next;
	int x;
	int y;
} food;

int keyLoop();
int guiLoop();
snake *createSnake();
void printSnake(snake *s);
void createFood(food *f);
void printFood(food *f);
int checkCollision(snake *s, food *f);
void eat(snake *s, food *f);

int frame = 0;

int dir = KEY_LEFT;



int main()
{	
	pthread_t gui, key;
	
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

snake *createSnake(){
	snake_part *body0 = malloc(sizeof(snake_part));
	snake_part *body1 = malloc(sizeof(snake_part));
	snake_part *body2 = malloc(sizeof(snake_part));
	snake *s = malloc(sizeof(snake));
	s->head=body0;
	s->butt=body2;
	body0->prev=NULL;	 	body0->next=body1;		body0->x = 10;			body0->y = LINES / 2;
	body1->prev=body0;		body1->next=body2;		body1->x = 11;			body1->y = LINES / 2;
	body2->prev=body1;		body2->next=NULL;		body2->x = 12;			body2->y = LINES / 2;
	return s;
}

void eat(snake *s, food *f){
	snake_part *body0 = malloc(sizeof(snake_part));
	snake_part *body1 = malloc(sizeof(snake_part));
	snake_part *body2 = malloc(sizeof(snake_part));
	body0->prev=s->butt; 		body0->next=body1;		body0->x = s->butt->x;			body0->y = s->butt->y;
	body1->prev=body0;		body1->next=body2;		body1->x = s->butt->x;			body1->y = s->butt->y;
	body2->prev=body1;		body2->next=NULL;		body2->x = s->butt->x;			body2->y = s->butt->y;
	s->butt->next = body0;
	s->butt=body2;
}

void createFood(food *f){
	if(frame % FOOD_INTERVAL == 0){
		food *last = f;
		food *newFood = malloc(sizeof(food));
		while(last->next != NULL){
			last = last->next;
		}
		last->next = newFood;
		newFood->next = NULL;
		newFood->x = rand() % COLS;
		newFood->y = rand() % LINES;
	}	
}

void printSnake(snake *s){
	snake_part *p = s->head;
	mvprintw(p->y,p->x,"X");
	p = p->next;
	do{
		mvprintw(p->y,p->x,"X");
		p = p->next;
	}while(p != NULL);
	
}

void printFood(food *f){
	food *f2 = f;
	do{
		mvprintw(f2->y,f2->x,"F");
		f2 = f2->next;
	}while(f2 != NULL);
}

void moveSnake(snake *s){
	snake_part *last 		= s->butt;
	snake_part *sndLast 	= last->prev;
	snake_part *head		= s->head;
	sndLast->next = NULL;

	last->next = head;
	last->prev = NULL;

	head->prev = last;

	s->butt = sndLast;
	s->head = last;

	head					= s->head;
	snake_part *sndHead		= head->next;
	int x = sndHead->x;
	int y = sndHead->y;
	switch(dir){
		case KEY_LEFT:
			x--;
			if(x < 0){
				x = COLS;
			}
			break;	
		case KEY_RIGHT:
			x = (x + 1) % COLS;
			break;	
		case KEY_UP:
			y--;
			if(y < 0){
				y = LINES;
			}
			break;	
		case KEY_DOWN:
			y = (y + 1) % LINES;
			break;	
	}
	head->x = x;
	head->y = y;
}

int guiLoop(){
	snake *s = createSnake();
	food *f = malloc(sizeof(food));
	f->next = NULL;
	f->x = 10;
	f->y = 10;
	frame=0;
	srand(time(0));
	int game =1;
	while(1){
		erase();
		if(game){
			createFood(f);
			moveSnake(s);
			if(checkCollision(s, f) < 0){
				endwin();	
				printf("collision!\n");
				freeMemory(s, f);
				return;
			}
			printSnake(s);
			printFood(f);
		}
		refresh();					/* Print it on to the real screen */
		usleep(1000*REDRAW_INTERVAL);
		frame++;
	}

}

int freeMemory(snake *snake_head, food *food_head) {
	snake_part *s = snake_head->head;
	while(s != NULL) {
		snake *next = s->next;
		free(s);
		s = next;
	}
	free(snake_head);
	food *f = food_head;
	while(f != NULL) {
		food *next = f->next;
		free(f);
		f = next;
	}
}

int checkCollision(snake *s, food *f){
	food *f2 = f;
	snake_part *p = s->head->next;
	int i = 0;

	do{
		if(p->x == s->head->x & p->y == s->head->y){
			return -1;

		}
		p = p->next;
		i++;
	}while(p != NULL);


	do{
		if(f2->x == s->head->x && f2->y == s->head->y){
			eat(s,f2);
		}
		f2 = f2->next;
	}while(f2 != NULL);
	return 0;
}


int keyLoop(){
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL); 
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
	int ch;
	int run = 1;
	while(run){
		ch = getch();
		switch(ch)
		{	
			case KEY_ESCAPE:
				run = 0;
				break;
			case KEY_LEFT:
			case KEY_RIGHT:
			case KEY_UP:
			case KEY_DOWN:
				dir = ch;
				break;	
		}
		pthread_testcancel();

	}	
}

