CC=gcc

CFLAGS=-Wall -std=c17 -g


.PHONY: all countdown clean

all: countdown

countdown: countdown.c
	$(CC) $(CFLAGS) -o $@ $^

debug: countdown
	gdb ./countdown

clean:
	rm -f countdown
