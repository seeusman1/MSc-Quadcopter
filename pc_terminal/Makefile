
CC=gcc
CFLAGS = -g -Wall -lm 
EXEC = ./pc-terminal

all:
	$(CC) $(CFLAGS) pc_terminal.c interface/interface.c -o $(EXEC) -lpthread -lrt

run: all
	$(EXEC)