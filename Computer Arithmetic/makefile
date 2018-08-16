CC = gcc
CCFLAGS = -Wall

all: calc format

calc: calc.c calc.h
	$(CC) $(CCFLAGS) -g -o calc calc.c calc.h

format: format.c format.h
	$(CC) $(CCFLAGS) -g -o format format.c format.h

clean:
	rm calc format