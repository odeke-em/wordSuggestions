CC := gcc
CC_FLAGS := -O3 -g -Wall -Wextra

autoCorrect:	Makefile wordlist.txt wordSearch.h wSearch.c Node.h wordTransition.o spellCorrect.c workSplitter.h workSplitter.c
	$(CC) $(CC_FLAGS) -DALLPRINTWORDS wSearch.c wordTransition.o -o autoCorrect -lpthread
	$(CC) $(CC_FLAGS) spellCorrect.c wordTransition.o -o spellCorrect -lpthread
	$(CC) $(CC_FLAGS) wordTransition.o workSplitter.c -o workSplitter -lpthread

wordTransition.o:   wordTransition.h wordTransition.c Makefile
	$(CC) -c wordTransition.c -o wordTransition.o
clean:
	rm autoCorrect spellCorrect workSplitter *.o
