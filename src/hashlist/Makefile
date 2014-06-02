CC := gcc

radLoadWords:	radLoadWords.c radTrie.o element.o wordTransition.o
	$(CC) -DTEST_LOAD_WORDS radTrie.o  wordTransition.o element.o radLoadWords.c -o rd

radTrie.o:	radTrie.c
	$(CC) -c radTrie.c -o radTrie.o

wordTransition.o:   hashlist.o wordTransition.c wordTransition.h
	$(CC) -c wordTransition.c -o wordTransition.o

hashlist.o:	hashList.c hashList.h
	$(CC) -c hashList.c -o hashlist.o

element.o:	element.c
	$(CC) -c element.c -o element.o

clean:	  
	rm -f *.o loadWords
