CC = gcc
CFLAGS = -Wall

all:
	$(CC) $(CFLAGS) server.c -o server
	$(CC) $(CFLAGS) client.c -o client

testserver:
	./server 127.0.0.1 8080

user1:
	./client user1 127.0.0.1:8080

user2:
	./client user2 127.0.0.1:8080

clean:
	rm -f server client