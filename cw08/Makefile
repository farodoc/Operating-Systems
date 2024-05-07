CC = gcc
CFALGS = -Wall

all:
	rm -rf /dev/mqueue/*
	$(CC) $(CFLAGS) -o printers_system printers_system.c -lrt -lpthread
	$(CC) $(CFLAGS) -o users_system users_system.c -lrt -lpthread

clean:
	rm -f printers_system users_system