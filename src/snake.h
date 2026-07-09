#ifndef SNAKE_H
#define SNAKE_H

#define _XOPEN_SOURCE_EXTENDED 1
#include <ncurses.h>
#include <wchar.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>

#define KEY_ESCAPE 		27 
#define REDRAW_INTERVAL		100
#define FOOD_INTERVAL		20

#define N_FOOD_TYPES 		7

/* Type definitions */
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
	struct food_struct* prev;
	int x;
	int y;
	int type;
} food;

typedef struct food_chain_struct {
	food *head;
	food *butt;
} food_chain;

typedef struct game_state_struct {
	int frame;
	int lives;
	int points;
	int level;
	int field_width;
	int field_height;
} game_state;

/* External variables */
extern wchar_t *food_symbols[];
extern game_state *state;
extern int dir;

/* Function declarations */
int keyLoop();
int guiLoop();
snake *createSnake();
void printSnake(snake *s);
void createFood(food_chain *fc);
void printFood(food_chain *fc);
int checkCollision(snake *s, food_chain *fc);
void eat(snake *s);
void moveSnake(snake *s);
void initGameState();
void printGameState();
int getRedrawInterval();
void incrementLevel();
int freeMemory(snake *snake_head, food_chain *fc);

#endif /* SNAKE_H */
