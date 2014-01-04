// Author: Emmanuel Odeke <odeke@ualberta.ca>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "errors.h"
#include "hashList.h"
#include "wordTransition.h"

// #define DEBUG

void initEditStat(EditStat *est) {
  if (est != NULL) {
    est->moves = est->inplace = est->deletions = 0;
    est->reuses = est->stringLen = est->additions = 0;
  }
}

inline EditStat *allocEditStat() { 
  return (EditStat *)malloc(sizeof(EditStat)); 
}

EditStat *allocAndInitEditStat() {
  EditStat *freshEditStat = allocEditStat();
  if (freshEditStat == NULL) {
    raiseError("Run out of memory");
  } else {
    initEditStat(freshEditStat);
  }

  return freshEditStat;
}

void printStat(const EditStat *est) {
  if (est != NULL) {
    printf(
    "\033[32mInplace: %d Moves: %d\nReuses: %d Deletions: %d\nAdditions: %d\033[00m\n",
      est->inplace, est->moves, est->reuses, est->deletions, est->additions
    );
  }
}

EditStat *getEditStats(const char *subject, const char *base) {
  if (subject == NULL || base == NULL) return NULL;
  HashList *baseIndices = NULL;

  int i, baseLen = strlen(base);

  // Note 'a' - 'A' to account for collisions that will result from 
  // alphabetic characters wrapping over the small hashlist size
  baseIndices = initHashListWithSize(baseIndices, baseLen + 'a' - 'A');

  baseIndices->allowCollisions = True;
  int alphaIndex = 0;
  for (i=0, alphaIndex=0; i < baseLen; ++i) {
    if (isalpha(base[i])) {
      int *indexCopy = (int *)malloc(sizeof(int));
      *indexCopy = alphaIndex;
      insertElem(baseIndices, indexCopy, tolower(base[i])-'a');
      ++alphaIndex;
    }
  }

  EditStat *est = allocAndInitEditStat();
  int subjectLen = strlen(subject);

#ifdef DEBUG
  printf("SubjectLen: %d\n", subjectLen);
#endif
  est->stringLen = subjectLen;

  for (i=subjectLen-1; i >= 0; --i) {
    if (! isalpha(subject[i])) continue;

    int subIndex = tolower(subject[i]) - 'a';
    Element **found = get(baseIndices, subIndex);
    if (*found != NULL) {
      Element *trav = *found;
    #ifdef DEBUG
      printf("found: %p ch: %c dTag: %d\n", trav, subject[i], trav->dTag);
    #endif
      while (trav != NULL && trav->dTag != False) { 
    #ifdef DEBUG
	printf("it: %p ch: %c dTag: %d\n", trav, subject[i], trav->dTag);
    #endif
	trav = trav->next;
      }
    #ifdef DEBUG
      printf("trav now: %p ch: %c\n", trav, subject[i]);
    #endif
      if (trav != NULL) {
	++est->reuses;
	trav->dTag = True;

	int storedIndex = *(int *)trav->value;
	if (storedIndex == i) {
	#ifdef DEBUG
	  printf("Reusing: %c at %d\n", subject[i], i);
	#endif
	  ++est->inplace;
	} else {
	#ifdef DEBUG
	  printf("Move %c from %d to %d\n", subject[i], i, storedIndex);
	#endif
	  ++est->moves;
	}

      #ifdef DEBUG_STORED_INDICES
	printf("Found %c at i %d: %d\n", subject[i], i, storedIndex);
      #endif
      }
    } else { // Element not in base
    #ifdef DEBUG
      printf("Delete %c from %d\n", subject[i], i);
      ++est->deletions;
    #endif
    }
  }

  int nValueFrees = destroyHashList(baseIndices);
  est->additions += nValueFrees; // Elements leftover and not matched by subject
  return est;
}

int getRank(const char *query, const char *from) {
  int rank = -1000; // Arbitrary most negative

  EditStat *et = getEditStats(query, from);
  if (et != NULL) {
    rank = (et->inplace*3)+(et->moves*2)+((et->deletions+et->additions)*-1);

    free(et);
  }

  return rank;
}

#ifdef SAMPLE
int main() {

  char *w = "googre\0", *base[] = {
    "monk\0", "bolton\0", "goo gle\0"
  };

  char **trav = base, **end = base + sizeof(base)/sizeof(base[0]);
  int setRank = getRank(w, w);
  while (trav != end) {
    printf("\033[33mTo get %s from %s\033[00m\n", w, *trav);
    int rank = getRank(*trav, w); //w, *trav);
    printf("Base: %d Rank : %d\n", setRank, rank);
    ++trav;
  }

  return 0;
}
#endif
