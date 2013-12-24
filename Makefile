GCC := gcc
FLAGS := -Wall -g -fPIC
LIBNAME := libaCorrect.so.1

autoCorrect: libaCorrect src/autoCorrect.c
	$(CC) $(FLAGS) src/autoCorrect.c -o exec/autoCorrect -ldl

libaCorrect:	  trie.o hashlist.o loadwords.o wordTransition.o estimateNWords.o
	$(CC)  -shared -Wl,-soname,$(LIBNAME) exec/trie.o exec/hashlist.o exec/loadWords.o exec/estimateNWords.o exec/wordTransition.o -o exec/libaCorrect.so.1 -lc

trie.o:	  src/trie/Trie.*
	$(CC) $(FLAGS) -c src/trie/Trie.c -o exec/trie.o

hashlist.o: src/hashlist/hashList.*
	$(CC) $(FLAGS) -c src/hashlist/hashList.c -o exec/hashlist.o

loadwords.o:  src/hashlist/loadWords.* estimateNWords.o
	$(CC) $(FLAGS) -c src/hashlist/loadWords.c -o exec/loadWords.o

wordTransition.o: src/hashlist/wordTransition.*
	$(CC) $(FLAGS) -c src/hashlist/wordTransition.c -o exec/wordTransition.o

estimateNWords.o:  src/hashlist/estimateNWords.c
	$(CC) $(FLAGS) -c src/hashlist/estimateNWords.c -o exec/estimateNWords.o

clean:
	rm exec/*.o exec/autoCorrect exec/*.so.*
