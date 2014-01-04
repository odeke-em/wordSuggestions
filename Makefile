GCC := gcc
FLAGS := -Wall -g -fPIC
CFLAGS := -Wall
LIBNAME := libaCorrect.so.1
LIBS := $(shell pkg-config --libs gtk+-2.0)
INCS := $(shell pkg-config --cflags gtk+-2.0)

all:	main cliCorrect

main: libaCorrect src/main.c
	$(CC) $(FLAGS) src/main.c -o exec/main -ldl $(LIBS) $(INCS) $(CFLAGS)

cliCorrect: libaCorrect src/cliCorrect.c
	$(CC) $(FLAGS) src/cliCorrect.c -o exec/cliCorrect -ldl $(CFLAGS)


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
	rm exec/*.o exec/main exec/*.so.*
