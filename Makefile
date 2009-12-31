CC = gcc

all:
	$(CC) -Wall -pedantic -Os das.c -o das

.PHONY: clean
clean:
	rm -f *.o das
