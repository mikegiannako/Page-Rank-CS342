CC = gcc
CFLAGS = -Wall -g -pthread

all: main

main: main.o random.o node.o
	$(CC) $(CFLAGS) $^ -o page_rank.exe

%.o:%.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean_results:
	-rm -f *.csv

clean:
	-rm -f *.exe *.o
