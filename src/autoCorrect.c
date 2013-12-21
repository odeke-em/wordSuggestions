// Author: Emmanuel Odeke <odeke@ualberta.ca>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "trie/Trie.h"
#include "hashlist/errors.h"
#include "hashlist/hashList.h"
#include "hashlist/loadWords.h"

#define tolower(x) (x | 'a' - 'A')
#define WORD_INCREMENT_LEN 10

int main() {
  HashList *dict = loadWordsInFile("./resources/wordlist.txt");

#ifdef DEBUG
  printf("Dict: %p\n", dict);
#endif

  Trie *memoizeTrie = createTrie(0);

  FILE *ifp = fopen("src/test.c", "r");
  while (! feof(ifp)) {
    char *inW = getWord(ifp);

    if (inW != NULL) {
      if  (searchTrie(memoizeTrie, inW) == -1) {
	// Word not yet discovered
	Element *match = getCloseMatches(inW, dict, 0.8);
	printf("%s {\n", inW);
	while (match != NULL) {
	  printf("\t%s\n", (char *)match->value);
	  match = getNext(match);
	} 

	printf("}\n");

	// Mark the word now as discovered
	memoizeTrie = addSequence(memoizeTrie, inW);
      }

      free(inW);
    }
  }

#ifdef DEBUG
  printf("Done ici");
#endif

  // CleanUp
  destroyHashList(dict);
  memoizeTrie = destroyTrie(memoizeTrie);

  if (ifp != NULL) fclose(ifp);
  return 0;
}
