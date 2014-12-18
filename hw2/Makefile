CC=gcc
CFLAGS=-g -Wall
TARGET=organizer judge player

.PHONY: all clean

all: $(TARGET)

organizer: organizer.c
	$(CC) $(CFLAGS) organizer.c -o organizer

judge: judge.c
	$(CC) $(CFLAGS) judge.c -o judge

player: player.c
	$(CC) $(CFLAGS) player.c -o player

clean:
	rm -f $(TARGET) *.FIFO
