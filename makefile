CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -Werror -g
RELEASEFLAGS=-O2 -DNDEBUG
DBGFLAGS = -fsanitize=address -fsanitize=undefined -DDEBUG
SRC=main.c lcache.c
OBJ=$(SRC:.c=.o)
TARGET=LRUcache

all: $(TARGET)

release: CFLAGS= $(RELEASEFLAGS)
release: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) $(DBGFLAGS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(DBGFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)

rebuild: clean all

.PHONY: all release run clean rebuild
