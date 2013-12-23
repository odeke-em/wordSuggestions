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

int main(int argc, char *argv[]) {
  const char *dictPath = "./resources/wordlist.txt";
  if (argc >= 2) {
    dictPath = argv[1];
  }

  HashList *dict = loadWordsInFile(dictPath);
  if (dict == NULL) {
    fprintf(stderr, "FilePath :: \033[32m%s\033[00m\n", dictPath);
    return -1;
  }

  FILE *ifp = stdin; // By default we shall read from standard input
  if (argc >= 3) {
    ifp = fopen(argv[2], "r");
    if (ifp == NULL) { // Revert to stdin
      fprintf(
       stderr, "\033[94mWe'll be reading words from standard input\033[00m\n"
      );
      ifp = stdin;
    }
  }

  float thresholdMatch = 0.8;
  if (argc >= 4) {
    if (sscanf(argv[3], "%f", &thresholdMatch) != 1) {
    #ifdef DEBUG
      raiseWarning("Couldn't parse the threshold rank");;
    #endif
    }
  }

#ifdef DEBUG
  printf("Dict: %p\n", dict);
#endif

  Trie *memoizeTrie = createTrie(0);

  printf("Threshold rank: %.2f\n", thresholdMatch);
  while (! feof(ifp)) {
    char *inW = getWord(ifp);
    if (inW != NULL && searchTrie(memoizeTrie, inW) == -1) {
      // Word already discovered
      Element *match = getCloseMatches(inW, dict, thresholdMatch);
      printf("%s {\n", inW);
      while (match != NULL) {
	printf("\t%s :: %d\n", (char *)match->value, match->rank);
	match = getNext(match);
      } 
      printf("}\n");
      memoizeTrie = addSequence(memoizeTrie, inW);
    }

    if (inW != NULL) free(inW);
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
