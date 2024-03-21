CXX = gcc
CFLAGS = -Wall -g

.PHONY: all clean

all: countdown

countdown: countdown.c
	$(CXX) $(CFLAGS) -o countdown countdown.c

clean:
	rm countdown