CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude
SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
EXEC = tsp

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lm

# Debug build (adds -DDEBUG)
debug: CFLAGS += -DDEBUG -g
debug: clean $(EXEC)
	@echo "- Built with DEBUG mode enabled -"

clean:
	rm -f src/*.o $(EXEC)