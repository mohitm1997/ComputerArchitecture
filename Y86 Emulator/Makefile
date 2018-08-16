CC = gcc
CCFLAGS = -Wall

all: y86emul y86dis

y86emul: y86emul.c y86emul.h
	$(CC) $(CCFLAGS) -g -o y86emul y86emul.c y86emul.h

y86dis: y86dis.c y86dis.h
	$(CC) $(CCFLAGS) -g -o y86dis y86dis.c y86dis.h

clean:
	rm y86emul y86dis