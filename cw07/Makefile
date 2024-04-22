CC = gcc
CFLAGS = -Wall

all:
	rm -rf /dev/mqueue/*
	$(CC) $(CFLAGS) client.c -o client
	$(CC) $(CFLAGS) server.c -o server

clean:
	rm -rf /dev/mqueue/*
	rm -f client server *.txt