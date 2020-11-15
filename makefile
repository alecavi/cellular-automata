CC=gcc
CFLAGS=-Wall -Wextra -Werror -pedantic

cellgame: cellular_automata.c
	$(CC) $(CFLAGS) $< -o $@ 