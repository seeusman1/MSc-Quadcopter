
CC=gcc
CFLAGS = -g -Wall -lm 
EXEC = ./pc-terminal

all:
	$(CC) $(CFLAGS) pc_terminal.c interface/interface.c messaging/messaging.c -o $(EXEC) -lpthread

run: all
	$(EXEC)