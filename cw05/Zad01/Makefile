CC = gcc
CFLAGS = -Wall

all:
	$(CC) $(CFLAGS) signals.c -o signals


test_none:
	./signals none

test_ignore:
	./signals ignore

test_handler:
	./signals handler

test_mask:
	./signals mask


clean:
	rm -f signals