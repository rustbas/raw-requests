CC=gcc
# CFLAGS=-Wall -Wextra -pedantic -ansi

all: main

main: main.c
	$(CC) $(CFLAGS) main.c -o main.out
