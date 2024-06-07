CC = gcc
CFLAGS = -Wall

all:
	$(CC) $(CFLAGS) server.c -o server
	$(CC) $(CFLAGS) client.c -o client

testserver:
	./server 127.0.0.1 10000

user1:
	./client user1 127.0.0.1:10000

user2:
	./client user2 127.0.0.1:10000

user3:
	./client user3 127.0.0.1:10000

clean:
	rm -f server client