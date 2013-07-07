CC := gcc
CC_FLAGS := -O3 -g -Wall

autoCorrect:	Makefile wordlist.txt wordSearch.h wSearch.c Node.h wordTransition.o
	$(CC) $(CC_FLAGS) -DALLPRINTWORDS wSearch.c wordTransition.o -o autoCorrect -lpthread

wordTransition.o:   wordTransition.h wordTransition.c Makefile
	$(CC) -c wordTransition.c -o wordTransition.o
clean:
	rm autoCorrect *.o
