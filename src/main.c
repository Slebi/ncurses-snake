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

	pthread_join(key,NULL);
	endwin();					/* End curses mode		  */

	return 0;
}

snake *createSnake(){
	snake_part *body = malloc(3*sizeof(snake_part));
	snake *s = malloc(sizeof(snake));
	s->head=&body[0];
	s->butt=&body[2];
	body[0].prev=NULL;	 		body[0].next=&body[1];		body[0].x = 10;			body[0].y = LINES / 2;
	body[1].prev=&body[0];		body[1].next=&body[2];		body[1].x = 11;			body[1].y = LINES / 2;
	body[2].prev=&body[1];		body[2].next=NULL;			body[2].x = 12;			body[2].y = LINES / 2;
	return s;
}

void eat(snake *s, food *f){
	snake_part *body = malloc(3*sizeof(snake_part));
	body[0].prev=s->butt; 		body[0].next=&body[1];		body[0].x = s->butt->x;			body[0].y = s->butt->y;
	body[1].prev=&body[0];		body[1].next=&body[2];		body[1].x = s->butt->x;			body[1].y = s->butt->y;
	body[2].prev=&body[1];		body[2].next=NULL;			body[2].x = s->butt->x;			body[2].y = s->butt->y;
	s->butt->next = &body[0];
	s->butt=&body[2];
}

void createFood(food *f){
	food *last = f;
	food *newFood = malloc(sizeof(food));
	if(last == NULL){
		last = newFood;
	}
	if(frame % FOOD_INTERVAL == 0){
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
				game=0;
			}
			printSnake(s);
			printFood(f);
		}
		refresh();					/* Print it on to the real screen */
		usleep(1000*REDRAW_INTERVAL);
		frame++;
	}
	free(s);
}

int checkCollision(snake *s, food *f){
	food *f2 = f;
	snake_part *p = s->head->next;
	int i = 0;

	do{
		if(p->x == s->head->x & p->y == s->head->y){
			endwin();	
			printf("collision!");		
			exit(0);
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
}


int keyLoop(){
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

	}	
}
