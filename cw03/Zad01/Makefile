SHELL := /bin/bash
CC = gcc
BIN = reverse to.txt reverse_* pomiar_zad_2.txt
FLAGS = -Wall

all:
	$(CC) $(FLAGS) reverse.c -o reverse
	touch to.txt
	touch pomiar_zad_2.txt

reverse_by_chars:
	$(CC) $(FLAGS) reverse.c -o reverse_by_chars -D BYCHARS
	touch to.txt
	touch pomiar_zad_2.txt
	echo -n > to.txt

reverse_by_blocks:
	$(CC) $(FLAGS) reverse.c -o reverse_by_blocks -D BYBLOCKS
	touch to.txt
	touch pomiar_zad_2.txt
	echo -n > to.txt

run:
	./reverse from.txt to.txt

#checks if program works correctly
test_diff:
	diff -s to.txt <(tac < from.txt | rev)

clean:
	rm $(BIN)