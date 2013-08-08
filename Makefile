CC := gcc
CC_FLAGS := -O3 -g -Wall -Wextra

autoCorrect:	Makefile resources/wordlist.txt include/wordSearch.h src/wSearch.c include/Node.h wordTransition.o src/spellCorrect.c include/workSplitter.h src/workSplitter.c
	$(CC) $(CC_FLAGS) -DALLPRINTWORDS src/wSearch.c exec/wordTransition.o -o exec/autoCorrect -lpthread
	$(CC) $(CC_FLAGS) src/spellCorrect.c exec/wordTransition.o -o exec/spellCorrect -lpthread
	$(CC) $(CC_FLAGS) exec/wordTransition.o src/workSplitter.c -o exec/workSplitter -lpthread

wordTransition.o:   include/wordTransition.h src/wordTransition.c Makefile
	$(CC) -c src/wordTransition.c -o exec/wordTransition.o
clean:
	cd exec && rm autoCorrect spellCorrect workSplitter *.o
