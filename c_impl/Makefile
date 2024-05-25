CC = gcc
CFLAGS = -g -Wall
LDFLAGS =

LDLIBS =

main: main.o

main.o: main.c enums.h structs.h row.h table.h constants.h

.PHONY: clean
clean:
	rm -f *.o a.out core main

.PHONY: all
all: clean main
