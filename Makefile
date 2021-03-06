CC := gcc
FLAGS := -Wall -g -fPIC
CFLAGS := -Wall
OS  := $(shell uname -s)
LIBNAME := libaCorrect.so.1
LIBS := $(shell pkg-config --libs gtk+-2.0)
INCS := $(shell pkg-config --cflags gtk+-2.0)

all:	main cliCorrect

main: libaCorrect src/main.c libLoading.o
	$(CC) $(FLAGS) exec/libLoading.o src/main.c -o exec/main -ldl $(LIBS) $(INCS) $(CFLAGS)

cliCorrect: libaCorrect src/cliCorrect.c libLoading.o
	$(CC) $(FLAGS) exec/libLoading.o src/cliCorrect.c -o exec/cliCorrect -ldl $(CFLAGS)

ifeq ($(OS), Darwin)
libaCorrect:	radTrie.o radLoadWords.o wordTransition.o element.o libLoading.o
	$(CC)  -shared -Wl,-install_name,$(LIBNAME) exec/element.o exec/radTrie.o exec/radLoadWords.o  exec/wordTransition.o -o exec/libaCorrect.so.1 -lc 
else
libaCorrect:	radTrie.o radLoadWords.o wordTransition.o element.o libLoading.o
	$(CC)  -shared -Wl,-soname,$(LIBNAME) exec/element.o exec/radTrie.o exec/radLoadWords.o  exec/wordTransition.o -o exec/libaCorrect.so.1 -lc 
	
endif

radTrie.o:	src/hashmap/radTrie.c element.o
	$(CC) $(FLAGS) -c src/hashmap/radTrie.c -o exec/radTrie.o

libLoading.o:	 src/libLoading/libLoading.*
	$(CC) $(FLAGS) -c src/libLoading/libLoading.c -o exec/libLoading.o -ldl

element.o:	src/hashmap/element.*
	$(CC) $(FLAGS) -c src/hashmap/element.c -o exec/element.o

radLoadWords.o:  src/hashmap/radLoadWords.* element.o radTrie.o
	$(CC) $(FLAGS) -c src/hashmap/radLoadWords.c -o exec/radLoadWords.o

wordTransition.o: src/hashmap/wordTransition.* element.o
	$(CC) $(FLAGS) -c src/hashmap/wordTransition.c -o exec/wordTransition.o

clean:
	rm exec/*.o exec/main exec/*.so.*
