// Author: Emmanuel Odeke <odeke@ualberta.ca>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "errors.h"
#include "hashList.h"
#include "wordTransition.h"

#define DEBUG

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

  unsigned int hashSize = baseLen + 'a' - 'A'; 
  // Note 'a' - 'A' to account for collisions that will result from 
  // alphabetic characters wrapping over the small hashlist size

  baseIndices = initHashListWithSize(baseIndices, hashSize);

  for (i=0; i < baseLen; ++i) {
    int *indexCopy = (int *)malloc(sizeof(int));
    *indexCopy = i;
    insertElem(baseIndices, indexCopy, base[i]-'a');
  }

  EditStat *est = allocAndInitEditStat();
  int subjectLen = strlen(subject);

  est->stringLen = subjectLen;

  for (i=0; i < subjectLen; ++i) {
    int subIndex = subject[i] - 'a';
    Element **found = get(baseIndices, subIndex);
    if (*found != NULL) {
      ++est->reuses;
      Element *curHead = (*found);
      *found = (*found)->next;

      int storedIndex = *(int *)curHead->value;
      if (storedIndex == i) {
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

      if (curHead->value != NULL) {
	free(curHead->value);
      }

      free(curHead);
    } else { // Element not in base
    #ifdef DEBUG
      printf("Delete %c from %d\n", subject[i], i);
    #endif
      ++est->deletions;
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

#ifdef SAMPLE_WORD_TRANSITION
int main() {

  char *w = "emmanlue\0", *base[] = {
    "space\0", "doekee\0", "emmanuel\0", "talons\0", "hashlist\0"
  };

  char **trav = base, **end = base + sizeof(base)/sizeof(base[0]);
  while (trav != end) {
    printf("\033[33mTo get %s from %s\033[00m\n", w, *trav);
    EditStat *et = getEditStats(w, *trav);
    printStat(et);
    if (et != NULL) {
      free(et);
    }
    ++trav;
  }

  return 0;
}
#endif
