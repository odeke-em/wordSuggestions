CC := gcc

loadWords.o:    loadWords.c hashlist.o wordTransition.o
	    $(CC) -c loadWords.c -o loadWords.o

wordTransition.o:   hashlist.o wordTransition.c wordTransition.h
	    $(CC) -c wordTransition.c -o wordTransition.o

hashlist.o:   hashList.c hashList.h
	    $(CC) -c hashList.c -o hashlist.o


clean:	  
	    rm -f *.o loadWords
