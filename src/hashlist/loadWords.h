#ifndef _LOAD_WORDS_H
#define _LOAD_WORDS_H
  #include "errors.h"
  #include "hashList.h"
  #include "wordTransition.h"

    typedef struct {
        char *query;
        unsigned int ownRank;
        double threshold;
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
