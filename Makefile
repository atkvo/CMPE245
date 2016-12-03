PROJNAME = engine
# SRCS = engine.c main.c
SRCS = engine.c GHmatrix.c lbc.c

CC = gcc

CFLAGS = -I.
CFLAGS += -g3

run: build
	./$(PROJNAME).o

build:
	gcc $(CFLAGS) $(SRCS) -o $(PROJNAME).o

clean:
	rm -f $(PROJNAME).o

rebuild: clean build
