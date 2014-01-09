#ifndef _LOAD_WORDS_H
#define _LOAD_WORDS_H
  #include "errors.h"
  #include "hashList.h"
  #include "wordTransition.h"

  char *getWord(FILE *ifp, int *lenStorage);
  HashList *loadWordsInFile(const char *filePath);
  Element *matches(
    const char *query, HashList *dict, 
    const unsigned int ownRank, const double threshHold
  );
  Element *getCloseMatches(
    const char *query, HashList *dict, const double percentMatch
  );
#endif
