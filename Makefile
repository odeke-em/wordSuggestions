GCC := gcc

wordSuggestion: wMatch.c trie.o hashlist.o loadwords.o wordTransition.o
	$(CC) wMatch.c exec/loadWords.o exec/trie.o exec/hashlist.o exec/wordTransition.o -o wMatch

trie.o:	  src/trie/Trie.*
	$(CC) -c src/trie/Trie.c -o exec/trie.o

hashlist.o: src/hashlist/hashList.*
	$(CC) -c src/hashlist/hashList.c -o exec/hashlist.o

loadwords.o:  src/hashlist/loadWords.*
	$(CC) -c src/hashlist/loadWords.c -o exec/loadWords.o

wordTransition.o: src/hashlist/wordTransition.*
	$(CC) -c src/hashlist/wordTransition.c -o exec/wordTransition.o

clean:
	rm exec/*.o
