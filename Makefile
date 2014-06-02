GCC := gcc
FLAGS := -Wall -g -fPIC
CFLAGS := -Wall
LIBNAME := libaCorrect.so.1
LIBS := $(shell pkg-config --libs gtk+-2.0)
INCS := $(shell pkg-config --cflags gtk+-2.0)

all:	main cliCorrect

main: libaCorrect src/main.c libLoading.o
	$(CC) $(FLAGS) exec/libLoading.o src/main.c -o exec/main -ldl $(LIBS) $(INCS) $(CFLAGS)

cliCorrect: libaCorrect src/cliCorrect.c libLoading.o
	$(CC) $(FLAGS) exec/libLoading.o src/cliCorrect.c -o exec/cliCorrect -ldl $(CFLAGS)


libaCorrect:	  trie.o hashlist.o loadwords.o wordTransition.o
	$(CC)  -shared -Wl,-soname,$(LIBNAME) exec/element.o exec/trie.o exec/hashlist.o exec/loadWords.o  exec/wordTransition.o -o exec/libaCorrect.so.1 -lc 

libLoading.o:	 src/libLoading/libLoading.*
	$(CC) $(FLAGS) -c src/libLoading/libLoading.c -o exec/libLoading.o -ldl

trie.o:	  src/trie/Trie.*
	$(CC) $(FLAGS) -c src/trie/Trie.c -o exec/trie.o

element.o:	src/hashlist/element.*
	$(CC) $(FLAGS) -c src/hashlist/element.c -o exec/element.o

hashlist.o: src/hashlist/hashList.* element.o
	$(CC) $(FLAGS) -c src/hashlist/hashList.c -o exec/hashlist.o

loadwords.o:  src/hashlist/loadWords.*
	$(CC) $(FLAGS) -c src/hashlist/loadWords.c -o exec/loadWords.o

wordTransition.o: src/hashlist/wordTransition.*
	$(CC) $(FLAGS) -c src/hashlist/wordTransition.c -o exec/wordTransition.o

clean:
	rm exec/*.o exec/main exec/*.so.*
