CC := gcc
CC_FLAGS := -O3 -g -Wall

autoCorrect:	Makefile wordlist.txt wordSearch.h wSearch.c Node.h wordTransition.o spellCorrect.c
	$(CC) $(CC_FLAGS) -DALLPRINTWORDS wSearch.c wordTransition.o -o autoCorrect -lpthread
	$(CC) $(CC_FLAGS) spellCorrect.c wordTransition.o -o spellCorrect -lpthread

wordTransition.o:   wordTransition.h wordTransition.c Makefile
	$(CC) -c wordTransition.c -o wordTransition.o
clean:
	rm autoCorrect spellCorrect *.o
