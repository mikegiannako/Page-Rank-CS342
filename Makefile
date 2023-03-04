CC = gcc
CFLAGS = -Wall -g -pthread
.PHONY: road
all: main

main: main.o random.o node.o
	$(CC) $(CFLAGS) $^ -o page_rank.exe

%.o:%.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	-rm -f *.exe *.o
