CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lncursesw -lpthread
TARGET = snake
SRC_DIR = src
SOURCES = $(SRC_DIR)/main.c $(SRC_DIR)/game_logic.c

.PHONY: all clean run valgrind

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(LDFLAGS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

valgrind: $(TARGET)
	valgrind ./$(TARGET)
