#ifndef _LOAD_WORDS_H
#define _LOAD_WORDS_H
  #include "errors.h"
  #include "hashList.h"
  #include "wordTransition.h"
  #define AVERAGE_WORD_LEN 8 // Arbitrary estimate

  typedef struct {
    char *query;
    int threshold;
    unsigned int ownRank;
    Element **start, **end, **absEnd; 
  } MatchUnit;

  void *subUnitMatch(void *data);
  Element *mergeEL(Element *from, Element *to);

  Element *matchesOnUniThreadOnMultiThread(
    const char *query, HashList *dict, const unsigned int ownRank,
    const double percentMatch, const unsigned int thCount
  );

  char *getWord(FILE *ifp, int *lenStorage);
  HashList *loadWordsInFile(const char *filePath);
  Element *matchesOnUniThread(
    const char *query, HashList *dict, 
    const unsigned int ownRank, const double threshHold
  );
  Element *getCloseMatches(
    const char *query, HashList *dict, const double percentMatch
  );
#endif
