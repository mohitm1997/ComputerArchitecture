CC = gcc
CCFLAGS = -m32 -Wall

formula: formula.c formula.h nCr.h nCr.s
	$(CC) $(CCFLAGS) -g formula.c nCr.s -o formula

clean:
	rm formula