## Compile and start
`gcc -o snake ./src/main.c ./src/game_logic.c -lncursesw -lpthread && ./snake`

### Valgrind
`gcc -o snake ./src/main.c ./src/game_logic.c -lncursesw -lpthread && valgrind ./snake`
