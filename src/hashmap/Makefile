CC := gcc

radTest:	radLoadWords.c radTrie.o element.o wordTransition.o
	$(CC) -DTEST_LOAD_WORDS radTrie.o element.o wordTransition.o radLoadWords.c -o $@

%.o: %.c
	$(CC) -c $< -o $@

clean:	  
	rm -f *.o rd
