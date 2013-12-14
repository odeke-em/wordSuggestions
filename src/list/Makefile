CC := gcc
LRU:	list.o list.h LRU.c
	$(CC) list.o LRU.c -o LRU

list.o:	  list.h list.c Makefile
	$(CC) -c list.c -o list.o

clean:	  
	rm *.o  LRU
