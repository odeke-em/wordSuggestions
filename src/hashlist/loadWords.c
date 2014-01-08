// Author: Emmanuel Odeke <odeke@ualberta.ca>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "hashList.h"
#include "loadWords.h"
#include "estimateNWords.h"
#include "wordTransition.h"

#define tolower(x) (x | ('a' - 'A'))

#define BUF_SIZ 10 // Starting element size for buffers

#define DEBUG_MATCH

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

HashList *loadWordsInFile(const char *filePath) {
  HashList *hl = NULL;

  if (filePath != NULL) {
    FILE *ifp = fopen(filePath, "r");
    LLInt dictSize = estimatedWordCount(ifp, 0);
    printf("\033[92mEstimated wordCount %lld", dictSize);
    dictSize *= 7; // Arbitrarily X 7

  #ifdef HANDLE_LIMITS
    if (dictSize > MAX_SAFETY_HASHLIST_SIZE) {
      dictSize  = MAX_SAFETY_HASHLIST_SIZE;
    }
  #endif

    printf(" Dict size: %lld\033[00m\n", dictSize);
    hl = initHashListWithSize(hl, dictSize);

    if (ifp == NULL) {
      raiseWarning("Invalid filePath");
      return NULL;
    }

    if (hl == NULL) {
      raiseError("No memory available to create a dict of size: "dictSize);

      const char *memLack = "No memory available to create a dict of size: ";
      char errMsg[(sizeof(memLack)/sizeof(memLack[0])) + 15];
      sprintf(errMsg, "%s%lld", memLack, dictSize);
      raiseWarning(errMsg);
      exit(-1);
    }

    int curLen = 0;
    int wordCount = 0;
    long long int totalLen = 0;
    while (! feof(ifp)) {
      char *wordIn = getWord(ifp, &curLen);
      if (wordIn != NULL) {
        insertElem(hl, wordIn, pjwCharHash(wordIn));
        totalLen += curLen;
        ++wordCount;
      }
      // Note we won't be freeing any memory yet as it
      // will be stored in the hashList
    }

  #ifdef DEBUG
    printf("averageWordLen: %d\n", wordCount ? totalLen/wordCount : 0);
  #endif
    hl->averageElemLen = (wordCount ? totalLen/wordCount : 0);
    fclose(ifp);
  }

  return hl;
}

// Do not free data returned from this function as it just contains pointers
// to data associated with the dictionary that will be explicitly freed
Element *matches(const char *query, HashList *dict, const int threshHold) {
  Element **matchList = NULL;
  if (query != NULL && dict != NULL) {
    Element *matchL = NULL;
    // First check if the query exists in the dict
    matchList = get(dict, pjwCharHash(query)); 
    if (*matchList == NULL) { // Not found time, to do ranking
      Element **trav = dict->list, **end = trav + getSize(dict);
      while (trav != end) {
	if (*trav != NULL && (*trav)->value != NULL) {
	  int rank = getRank(query, (*trav)->value);
	  if (rank >= threshHold) {
	    matchL = addToHeadWithRank(matchL, (*trav)->value, rank);
	  } 
	}
	++trav;
      }
    } else {
      matchL = *matchList;
    }

    return matchL;
  } else 
    return NULL;
}

Element *getCloseMatches(
  const char *query, HashList *dict, const double percentMatch
) {
  if (query == NULL || dict == NULL) {
    return NULL;
  } else {
    int ownRank = getRank(query, query);
    return matches(query, dict, ownRank * percentMatch);
  }
}

#ifdef SAMPLE_RUN
int main() {
  HashList *hl = loadWordsInFile("../../resources/wordlist.txt");
  HashList *curFileHL = loadWordsInFile(__FILE__);

  int i;
  printf("hl: %p\n", hl);
  printf("curFileHl: %p\n", curFileHL);
  for (i = 0; i < getSize(curFileHL); ++i) {
    if (curFileHL->list[i] != NULL) {
      char *value = (char *)(curFileHL->list[i]->value);
      Element *match = getCloseMatches(value, hl, 0.6);
      printf("For: %s %p\n", value, curFileHL->list[i]);
      while (match != NULL) {
	printf("\t%s\n", (char *)match->value);
	match = getNext(match);
      }
    }
  }

  destroyHashList(hl);
  destroyHashList(curFileHL);
  return 0;
}
#endif
