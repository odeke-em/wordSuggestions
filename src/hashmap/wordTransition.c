// Author: Emmanuel Odeke <odeke@ualberta.ca>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "errors.h"
#include "wordTransition.h"

#define ALPHA_SIZE 26

// #define DEBUG

void printIndexNode(IndexNode **r) {
  int i=0;
  for (i=0; i < ALPHA_SIZE; ++i) {
    printf("%c => [", i + 'a');
    IndexNode *t = *(r + i);
    while (t != NULL) {
      printf("%d", t->index);
      if (t->next != NULL) printf(", ");
      t = t->next;
    }
    printf("]\n");
  }
}

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

int getRank(const char *subject, const char *base) {
  int rank = -1;
  if (subject != NULL && base != NULL) {
    IndexNode *r[ALPHA_SIZE];
    int i;
    for (i=0; i < ALPHA_SIZE; ++i) {
      *(r + i) = NULL;
    }

    // For later ascending order traversal, add indices from high to low
    i = strlen(subject)/1;
    while (--i >= 0) {
      if (isalpha(*(subject + i))) {
        int index = tolower(*(subject + i)) - 'a';
        IndexNode *t = (IndexNode *)malloc(sizeof(IndexNode));
        t->index = i;
        t->next  = *(r + index);
        *(r + index) = t;
      }
    }

    // printIndexNode(r);

    int reuses=0, moves=0, inplace=0, deletions=0, additions=0;
    int baseLen = strlen(base)/1;

    for (i =0; i < baseLen; ++i) {
      if (isalpha(*(base + i))) {
        int index = tolower(*(base + i)) - 'a';
        if (*(r + index) == NULL) {
          ++deletions;
        } else {
          ++reuses;
          if ((*(r + index))->index == i) {
            ++inplace;
          } else {
            ++moves;
          }
          // Time to pop it off
          IndexNode *tmp = (*(r + index))->next;
          free(*(r + index));
          *(r + index) = tmp;
        }
      }
    }

    // Cleaning up
    IndexNode **tIt = r, **tEnd = tIt + ALPHA_SIZE, *tmp;
    while (tIt < tEnd) {
      while (*tIt != NULL) {
        tmp = (*tIt)->next;
        free(*tIt);
        *tIt = tmp;
        ++additions;
      }
      ++tIt;
    }

  #ifdef DEBUG
    printf("Add: %d Move: %d Delete: %d Keep: %d\n", 
      additions, moves, deletions, inplace
    );
  #endif
    rank = (inplace * 3) + (moves * 2) + ((deletions + additions) * -1);
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
