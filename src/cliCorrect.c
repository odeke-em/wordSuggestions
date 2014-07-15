// Author: Emmanuel Odeke <odeke@ualberta.ca>
// Main For the auto correct program

#include <ctype.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "hashmap/errors.h"
#include "hashmap/element.h"
#include "hashmap/radTrie.h"
#include "hashmap/radLoadWords.h"

#define INTERACTIVE
#define THRESHOLD_RANK 0.65
#define WORD_INCREMENT_LEN 10
#define BUF_SIZ 10 // Starting element size for buffers

#define tolower(x) (x | ('a' - 'A'))

#define checkLoading(handle, funcPtr, libKey) {\
  funcPtr = dlsym(handle, libKey);\
  if ((error = dlerror()) != NULL) {\
    fputs(error, stderr);\
    exit(-1);\
  }\
}

char *getWord(FILE *ifp, int *lenStorage) {
  int bufLen = BUF_SIZ;
  char *wordIn = (char *)malloc(sizeof(char) * bufLen);
  assert(wordIn);

  char c;
  int idx = 0;
  while ((c = getc(ifp)) != EOF) {
    if ((idx + 1) >= bufLen) {
      bufLen += BUF_SIZ;
      wordIn = (char *)realloc(wordIn, sizeof(char) * bufLen);
    }

    if (isalpha(c)) { 
      wordIn[idx++] = tolower(c);
    } else break;
  }

  if (idx) {
    wordIn[idx++] = '\0';
    wordIn = (char *)realloc(wordIn, sizeof(char) * idx);

    if (lenStorage) *lenStorage = idx;

    return wordIn;
  } else {
    free(wordIn);
    return NULL;
  }
}

void destroyRTrieWithMemLinearized(void *handle, RTrie *dict) {
    }

int main(int argc, char *argv[]) {
  void *handle = dlopen("./exec/libaCorrect.so.1", RTLD_LAZY);
  if (handle == NULL) {
    fputs(dlerror(), stderr);
    exit(-1);
  }

  // Function pointers declared here
  RTrie * (*dictFromFile)(const char *) = NULL; 
  void *(*getItem)(RTrie *, unsigned long int) = NULL;
  unsigned int (*pjwCharHasher)(const char *srcW) = NULL;
  Element *(*getMatches)(const char *, RTrie *, const double) = NULL;
  RTrie *(*putItem)(RTrie *, unsigned long int, void *, Bool) = NULL;
  Element *(*getNext)(Element *) = NULL;
  RTrie * (*destroyRTrie)(RTrie *rt) = NULL;

  char *error;
  // Loading the functions
  checkLoading(handle, getNext, "getNext");
  checkLoading(handle, dictFromFile, "fileToRTrie");
  checkLoading(handle, getMatches, "getCloseMatches");
  checkLoading(handle, pjwCharHasher, "pjwCharHash");
  checkLoading(handle, putItem, "put");
  checkLoading(handle, getItem, "get");

  const char *dictPath = "./resources/wordlist.txt";
  if (argc >= 2) {
    dictPath = argv[1];
  }
#ifdef INTERACTIVIE
  fprintf(stderr, "\033[93mDictionary path: %s\n", dictPath);
#endif

  RTrie *dict = dictFromFile(dictPath);
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

  RTrie *memoizeTrie = NULL;

  int indentLevel = 0;
  int curLen = 0;
  while (! feof(ifp)) {
    char *inW = getWord(ifp, &curLen);
    if (inW != NULL) {
      printf("%s ", inW);

      unsigned long int hValue = pjwCharHasher(inW);
      void *retr = getItem(memoizeTrie, hValue);

	  Element *match = getItem(memoizeTrie, hValue);
      if (retr == NULL) { // Word not yet discovered
        match = getMatches(inW, dict, thresholdMatch);
        memoizeTrie = putItem(memoizeTrie, hValue, match, False);
      }
	  printf(" {\n");
      indentLevel += 1;
	  while (match != NULL) {
	    printf("\t%*s :: %.2f\n", indentLevel, (char *)match->value, match->rank);
	    match = getNext(match);
      }
	  indentLevel = 0;
	  printf("%*s\n", indentLevel, " }");
      free(inW);
    }
  }

  printf("Done ici");

  // CleanUp
  checkLoading(handle, destroyRTrie, "destroyRTrie");
  memoizeTrie = destroyRTrie(memoizeTrie);

  if (dict != NULL) {
    char *error;
    LinearizedTrie * (*destroyLinearizedTrie)(LinearizedTrie *l) = NULL;
    checkLoading(handle, destroyLinearizedTrie, "destroyLinearizedTrie");

    dict->meta = destroyLinearizedTrie((LinearizedTrie *)dict->meta);
    dict->meta = NULL;

    dict = destroyRTrie(dict);
  }

  if (ifp != NULL)
    fclose(ifp);

  return 0;
}
