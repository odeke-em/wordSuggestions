// Author: Emmanuel Odeke <odeke@ualberta.ca>
// Main For the auto correct program

#include <ctype.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "trie/Trie.h"
#include "hashlist/errors.h"
#include "hashlist/hashList.h"
#include "hashlist/loadWords.h"

#define INTERACTIVE
#define THRESHOLD_RANK 0.80
#define WORD_INCREMENT_LEN 10

#define tolower(x) (x | 'a' - 'A')

#define checkLoading(handle, funcPtr, libKey) {\
  funcPtr = dlsym(handle, libKey);\
  if ((error = dlerror()) != NULL) {\
    fputs(error, stderr);\
    exit(-1);\
  }\
}

int main(int argc, char *argv[]) {
  void *handle = dlopen("./exec/libaCorrect.so.1", RTLD_LAZY);
  if (handle == NULL) {
    fputs(dlerror(), stderr);
    exit(-1);
  }

  char *(*getWord)(FILE *);
  Trie *(*destroyTrie)(Trie *);
  Trie *(*createTrie)();
  Element *(*getNext)(Element *);
  long int (*destroyHashList)(HashList *hl);
  int (*searchTrie)(Trie *tr, const char *);
  HashList * (*loadWordsInFile)(const char *); 
  Trie *(*addSequence)(Trie *tr, const char *);
  Element *(*getCloseMatches)(const char *, HashList *, const double);

  char *error;
  // Loading the functions
  checkLoading(handle, getWord, "getWord");
  checkLoading(handle, getNext, "getNext");
  checkLoading(handle, createTrie, "createTrie");
  checkLoading(handle, searchTrie, "searchTrie");
  checkLoading(handle, addSequence, "addSequence");
  checkLoading(handle, destroyTrie, "destroyTrie");
  checkLoading(handle, getCloseMatches, "getCloseMatches");
  checkLoading(handle, loadWordsInFile, "loadWordsInFile");
  checkLoading(handle, destroyHashList, "destroyHashList");

  const char *dictPath = "./resources/wordlist.txt";
  if (argc >= 2) {
    dictPath = argv[1];
  }
#ifdef INTERACTIVIE
  fprintf(stderr, "\033[93mDictionary path: %s\n", dictPath);
#endif

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
  } else {
    #ifdef INTERACTIVE
      fprintf(
	stderr, "\033[93mWe'll be reading from standard input\033[00m\n"
      );
    #endif
  }

  float thresholdMatch = THRESHOLD_RANK;
  if (argc >= 4) {
    if (sscanf(argv[3], "%f", &thresholdMatch) != 1) {
    #ifdef DEBUG
      raiseWarning("Couldn't parse the threshold rank");;
    #endif
    thresholdMatch = THRESHOLD_RANK;
    }
  }
 
#ifdef INTERACTIVE
  fprintf(stdout, "ThresholdMatch percentage: %.2f\n", thresholdMatch);
#endif

#ifdef DEBUG
  printf("Dict: %p\n", dict);
#endif

  Trie *memoizeTrie = createTrie();

  int indentLevel = 0;
  while (! feof(ifp)) {
    char *inW = getWord(ifp);
    if (inW != NULL) {
      printf("%s ", inW);
      if (searchTrie(memoizeTrie, inW) == -1) {
	printf(" {\n");
	// Word not yet discovered
	Element *match = getCloseMatches(inW, dict, thresholdMatch);
	while (match != NULL) {
	  printf("\t%*s :: %d\n", indentLevel, (char *)match->value, match->rank);
	  match = getNext(match);
	} 
	memoizeTrie = addSequence(memoizeTrie, inW);
	printf("%*s\n", indentLevel, " }");
	indentLevel = 0;
      } else {
	indentLevel += 1;
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
