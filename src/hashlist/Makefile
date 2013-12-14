CC := gcc

testHash:     hashlist.o  testHash.c
	    $(CC) testHash.c hashlist.o -o testHash

wordTransition.o:   hashlist.o wordTransition.c wordTransition.h
	    $(CC) -c wordTransition.c -o wordTransition.o

hashlist.o:   hashList.c hashList.h
	    $(CC) -c hashList.c -o hashlist.o

clean:	  
	    rm -f *.o
