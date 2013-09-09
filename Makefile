CC := gcc
CC_FLAGS := -O3 -g -Wall -Wextra

all:	Makefile autoCorrect workSplitter spellCorrect

spellCorrect:	Makefile wordLib.o wordTransition.o resources/wordlist.txt include/wordSearch.h src/spellCorrect.c
	$(CC) $(CC_FLAGS) -DINTERACTIVE exec/wordLib.o src/spellCorrect.c exec/wordTransition.o -o exec/spellCorrect -lpthread

workSplitter:	Makefile resources/wordlist.txt include/wordSearch.h include/Node.h wordTransition.o wordLib.o
	$(CC) $(CC_FLAGS) exec/wordLib.o exec/wordTransition.o src/workSplitter.c -o exec/workSplitter -lpthread

autoCorrect:	Makefile resources/wordlist.txt include/wordSearch.h src/autoCorrect.c include/Node.h wordTransition.o wordLib.o
	$(CC) $(CC_FLAGS) -DALLPRINTWORDS exec/wordLib.o src/autoCorrect.c exec/wordTransition.o -o exec/autoCorrect -lpthread

wordLib.o:    include/wordLib.h src/wordLib.c
	$(CC) $(CC_FLAGS) -c src/wordLib.c -o exec/wordLib.o

wordTransition.o:   include/wordTransition.h src/wordTransition.c Makefile
	$(CC) -c src/wordTransition.c -o exec/wordTransition.o

hashLib:    wordLib.o include/hashLib.h src/hashLib.c
	$(CC) -DSAMPLE_RUN exec/wordLib.o src/hashLib.c -o hashLib

stringLib:   wordLib.o src/custString.c include/custString.h
	$(CC) -c src/custString.c -o exec/custString.o

clean:
	cd exec && rm autoCorrect spellCorrect workSplitter *.o
