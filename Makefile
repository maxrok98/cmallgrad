CS=engine.c nn.c utils.c
CFLAGS=-Wall -ggdb
CLINK=-lm
CC=gcc

main: main.c $(CS)
	$(CC) $(CFLAGS) -o grad main.c $(CS) $(CLINK)
