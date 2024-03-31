CC = gcc
Flags = -Wall

all:
	$(CC) $(Flags) main.c -o main

test:
	mkdir -p test
	touch test/file1
	touch test/file2
	./main .
	./main test
	./main niemategokatalogu

clean:
	rm -rf main test