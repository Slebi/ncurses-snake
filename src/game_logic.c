#include "snake.h"

wchar_t *food_symbols[] = {
    L"🍎",
    L"🍕",
    L"🍔",
    L"🌮",
    L"🍣",
    L"🍩",
    L"🥗"
};

game_state *state;

int dir = KEY_LEFT;

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

void eat(snake *s){
	snake_part *body0 = malloc(sizeof(snake_part));
	snake_part *body1 = malloc(sizeof(snake_part));
	snake_part *body2 = malloc(sizeof(snake_part));
	body0->prev=s->butt; 		body0->next=body1;		body0->x = s->butt->x;			body0->y = s->butt->y;
	body1->prev=body0;		body1->next=body2;		body1->x = s->butt->x;			body1->y = s->butt->y;
	body2->prev=body1;		body2->next=NULL;		body2->x = s->butt->x;			body2->y = s->butt->y;
	s->butt->next = body0;
	s->butt=body2;
}

food* removeFood(food_chain* fc, food* f) {
	state->points += (10 + f->type) * state->level * state->level;
	food* prev = f->prev;
	if (f->prev != NULL && f->next != NULL) {
		f->prev->next = f->next;
		f->next->prev = f->prev;

	}

	if (f->prev == NULL && f->next != NULL) {
		//we are first food
		fc->head = f->next;
		f->next->prev = NULL;
	}

	if (f->prev != NULL && f->next == NULL) {
		//we are last food
		fc->butt = f->prev;
		f->prev->next = NULL;
	}

	if (f->prev == NULL && f->next == NULL) {
		//we are only food
		fc->head = NULL;
		fc->butt = NULL;
	}

	free(f);
	return prev;
}

void createFood(food_chain *fc){
	if(state->frame % FOOD_INTERVAL == 0){
		food *last = fc->head;

		food *newFood = malloc(sizeof(food));
		if (last != NULL) {
			while(last->next != NULL){
				last = last->next;
			}
			last->next = newFood;
		} else {
			//we are the first food
			fc->head = newFood;
		}

		newFood->next = NULL;
		newFood->prev = last;
		newFood->x = rand() % state->field_width;
		newFood->y = rand() % state->field_height;
		newFood->type = rand() % N_FOOD_TYPES;
		fc->butt = newFood;
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

void printFood(food_chain *fc){
	food *f2 = fc->head;
	do{
		mvaddwstr(f2->y, f2->x, L" ");
		mvaddwstr(f2->y, f2->x, food_symbols[f2->type]);
		f2 = f2->next;
	}while(f2 != NULL);
}

void printGameState() {
	for(int i = 0; i < COLS; i++ ) {
		mvprintw(LINES-2,i,"=");
	}
	char pointStr[5];
	sprintf(pointStr,"%i",state->points);
	mvaddwstr(LINES-1,COLS-15, L"💎 ");
	mvprintw(LINES-1,COLS-11,pointStr);
	mvaddwstr(LINES-1,COLS- 6, L"❤️ ");
	char liveStr[2];
	sprintf(liveStr,"%i",state->lives);
	mvprintw(LINES-1,COLS- 3,liveStr);
	mvaddwstr(LINES-1,COLS- 2, L"🪜 ");
	char levelStr[2];
	sprintf(levelStr,"%i",state->level);
	mvprintw(LINES-1,COLS,levelStr);
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
				x = state->field_width;
			}
			break;
		case KEY_RIGHT:
			x = (x + 1) % state->field_width;
			break;
		case KEY_UP:
			y--;
			if(y < 0){
				y = state->field_height;
			}
			break;
		case KEY_DOWN:
			y = (y + 1) % state->field_height;
			break;
	}
	head->x = x;
	head->y = y;
}

void initGameState(){
	state = malloc(sizeof(game_state));
	state->frame=0;
	state->points=0;
	state->lives=5;
	state->level=1;
	state->field_width = COLS - 1; //is this a ubuntu terminal requirement?
	state->field_height = LINES - 3; //game state info at bottom
}

int getRedrawInterval() {
	return REDRAW_INTERVAL / (1 + (state->level / 3));
}

void incrementLevel() {
	if (state->level == 1 && state->points > 300) {
		state->level = 2;
	}
	if (state->level == 2 && state->points > 900) {
		state->level = 3;
	}
	if (state->level == 3 && state->points > 2000) {
		state->level = 4;
	}
	if (state->level == 4 && state->points > 4000) {
		state->level = 5;
	}
	if (state->level == 5 && state->points > 15000) {
		state->level = 6;
	}
	if (state->level == 6 && state->points > 20000) {
		state->level = 7;
	}
}

int guiLoop(){
	initGameState();
	snake *s = createSnake();
	food_chain *fc = malloc(sizeof(food_chain));
	fc->head = NULL;
	fc->butt = NULL;
	createFood(fc);
	srand(time(0));

	while(state->lives > 0){
		int ch = getch();
		switch(ch)
		{
			case KEY_ESCAPE:
				state->lives = 0;
				break;
			case KEY_LEFT:
			case KEY_RIGHT:
			case KEY_UP:
			case KEY_DOWN:
				dir = ch;
				break;
		}

		erase();

		createFood(fc);
		moveSnake(s);
		if(checkCollision(s, fc) < 0){
			state->lives--;
		}
		printGameState();
		printSnake(s);
		printFood(fc);
		incrementLevel();
		refresh();					/* Print it on to the real screen */
		usleep(1000*getRedrawInterval());
		state->frame++;
	}
	endwin();
	printf("💎 %i\n", state->points);
	printf("🪜 %i\n", state->level);
	printf("🐢 🐢 G A M E    O V E R 🐢 🐢\n");
	freeMemory(s, fc);
	return;

}

int freeMemory(snake *snake_head, food_chain *fc) {
	snake_part *s = snake_head->head;
	while(s != NULL) {
		snake *next = s->next;
		free(s);
		s = next;
	}
	free(snake_head);
	food *f = fc->head;
	while(f != NULL) {
		food *next = f->next;
		free(f);
		f = next;
	}
	free(fc);
	free(state);
}

int checkCollision(snake *s, food_chain *fc){
	food *f2 = fc->head;
	snake_part *p = s->head->next;
	int i = 0;

	do{
		if(p->x == s->head->x & p->y == s->head->y){
			return -1;

		}
		p = p->next;
		i++;
	}while(p != NULL);


	while(f2 != NULL){
		if((f2->x == s->head->x || f2->x+1 == s->head->x) && f2->y == s->head->y){
			eat(s);
			f2 = removeFood(fc, f2);
		}
		if (f2 != NULL) {
			f2 = f2->next;
		}
	}
	return 0;
}
