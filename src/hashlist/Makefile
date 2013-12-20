CC := gcc

loadWords.o:    loadWords.c hashlist.o wordTransition.o
	    $(CC) -c loadWords.c -o loadWords.o

testHash:     hashlist.o  loadWords.o testHash.c wordTransition.o
	    $(CC) loadWords.o wordTransition.o testHash.c hashlist.o -o testHash

wordTransition.o:   hashlist.o wordTransition.c wordTransition.h
	    $(CC) -c wordTransition.c -o wordTransition.o

hashlist.o:   hashList.c hashList.h
	    $(CC) -c hashList.c -o hashlist.o

clean:	  
	    rm -f *.o loadWords
