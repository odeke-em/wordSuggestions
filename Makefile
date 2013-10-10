CC := gcc
CC_FLAGS := -O3 -g -Wall -Wextra

all:	Makefile autoCorrect workSplitter spellCorrect

testTree:	Makefile wordLib Tree wordTransition src/test.c wordSearch  Node
	$(CC) $(CC_FLAGS) exec/Node.o exec/wordLib.o exec/wordTransition.o exec/wordSearch.o exec/BTree.o src/test.c -o exec/test

spellCorrect:	Makefile wordLib wordTransition wordSearch resources/wordlist.txt src/spellCorrect.c Node utilityFuncs Tree
	$(CC) $(CC_FLAGS) -DINTERACTIVE exec/wordLib.o src/spellCorrect.c exec/wordTransition.o exec/BTree.o exec/wordSearch.o exec/utilityFuncs.o exec/Node.o -o exec/spellCorrect -lpthread

workSplitter:	Makefile resources/wordlist.txt include/Node.h wordTransition wordLib wordSearch Node utilityFuncs Tree
	$(CC) $(CC_FLAGS) exec/wordLib.o exec/wordTransition.o src/workSplitter.c exec/wordSearch.o exec/BTree.o exec/Node.o exec/utilityFuncs.o -o exec/workSplitter -lpthread

autoCorrect:	Makefile resources/wordlist.txt src/autoCorrect.c include/Node.h wordTransition wordLib wordSearch utilityFuncs Node Tree
	$(CC) $(CC_FLAGS) -DALLPRINTWORDS exec/wordLib.o src/autoCorrect.c exec/wordTransition.o exec/wordSearch.o exec/BTree.o exec/Node.o exec/utilityFuncs.o -o exec/autoCorrect -lpthread

wordLib:    include/wordLib.h src/wordLib.c
	$(CC) $(CC_FLAGS) -c src/wordLib.c -o exec/wordLib.o

wordTransition:   include/wordTransition.h src/wordTransition.c Makefile
	$(CC) -c src/wordTransition.c -o exec/wordTransition.o

hashLib:    wordLib include/hashLib.h src/hashLib.c
	$(CC) -DSAMPLE_RUN exec/wordLib.o src/hashLib.c -o hashLib

wordSearch: wordLib include/wordSearch.h src/wordSearch.c 
	$(CC) -DDEBUG -c src/wordSearch.c -o exec/wordSearch.o

stringLib:   wordLib src/custString.c include/custString.h
	$(CC) -c src/custString.c -o exec/custString.o

utilityFuncs:	include/customTypes.h src/utilityFuncs.c include/utilityFuncs.h
	$(CC) -c src/utilityFuncs.c -o exec/utilityFuncs.o

Tree:	  include/bTree.h src/bTree.c
	$(CC) -c src/bTree.c -o exec/BTree.o

Node:	  include/Node.h src/Node.c
	$(CC) -c src/Node.c -o exec/Node.o

clean:
	cd exec && rm autoCorrect spellCorrect workSplitter *.o
