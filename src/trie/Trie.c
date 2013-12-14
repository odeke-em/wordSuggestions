// Author: Emmanuel Odeke <odeke@ualberta.ca>

#include <stdio.h>
#include <stdlib.h>

#include "Trie.h"
#include "errors.h"

#ifdef DEBUG
  #undef DEBUG
#endif


Trie *createTrie(const int index) {
  Trie *freshTrie = allocTrie();
  if (freshTrie == NULL) {
    raiseError("Run-out of memory");
  }

  freshTrie->index = index;
  freshTrie->keys = (Trie **)malloc(sizeof(Trie *) * radixSize);

  if (freshTrie->keys == NULL) {
    raiseError("Run-out of memory");
  }

  freshTrie->radixSz = radixSize;

  int i;
  for (i=0; i < radixSize; ++i) {
    freshTrie->keys[i] = NULL;
  }

  return freshTrie;
}

Trie *destroyTrie(Trie *tr) {
#ifdef DEBUG
  printf("In %s\n", __func__);
#endif
  if (tr == NULL) return tr;

  if (tr->keys != NULL) {
    Trie **it = tr->keys;
    Trie **end = tr->keys+ tr->radixSz;
    while (it < end) {
      if (*it != NULL) {
	*it = destroyTrie(*it);
      }
      ++it;
    }

    free(tr->keys);
    tr->keys = NULL;
  }


  free(tr);
  tr = NULL;
  return tr;
}

Trie *addSequence(Trie *tr, const char *seq) {
#ifdef DEBUG
  printf("%s seq: %s\n", __func__, seq);
#endif
  if (tr == NULL || tr->keys == NULL) {
    raiseError("Cannot add elements to a NULL Trie");
  }

  if (seq != NULL || *seq != '\0') {
    int targetIndex = resolveIndex(*seq);
    if (targetIndex >= 0 && targetIndex < radixSize) {
      if (tr->keys[targetIndex] == NULL) {
	tr->keys[targetIndex] = createTrie(targetIndex);
      #ifdef DEBUG
	printf("New Trie alloc index: %d\n", targetIndex);
      #endif
      }

      tr->keys[targetIndex] = addSequence(tr->keys[targetIndex], seq+1);
    }
  }

  return tr;
}

int searchTrie(Trie *tr, const char *seq) {
  if (seq == NULL) return -1;
  else if (tr == NULL || tr->keys == NULL) return 0;

  if (*seq == '\0') return 1;

  int resIndex = resolveIndex(*seq);
  if (resIndex < 0 || resIndex >= radixSize) return -1;

  if (tr->keys[resIndex] == NULL) {
    return -1;
  } else {
    return searchTrie(tr->keys[resIndex], seq+1);
  }
}

inline int bitLen(unsigned int n) {
  int bLength = 1;
  while (n) { ++bLength; n >>= 1;}
  return bLength;
}

Trie *allocTrie() { 
  return (Trie *)malloc(sizeof(Trie)); 
}

int resolveIndex(const char c) {
  int resIndex = -1;
  if (isalpha(c)) {
    resIndex = c-radixStart;
  }

  return resIndex;
}
int main() {
  Trie *tR = createTrie(0);
  printf("tR->index: %d\n", tR->index);
  tR = addSequence(tR, "abc\0");
  tR = addSequence(tR, "mbc\0");
  tR = addSequence(tR, "mac\0");
  printf("tR->index: %d\n", tR->index);
  int found = searchTrie(tR, "mb\0");
  printf("\033[%dmFound: %d\033[00m\n", found == 1 ? 33 : 31, found);

  // Consume self
  FILE *ifp = fopen(__FILE__, "r");
  int BUF_STEP = 10, MAX_SINGLE_ALLOC_SZ = 60;
  char c;

  while (! feof(ifp)) {
    int i = 0, dBufSize = BUF_STEP;

    char *wordIn = (char *)malloc(sizeof(char) * dBufSize);

    while ((c = getc(ifp)) != EOF && isalpha(c) && i < MAX_SINGLE_ALLOC_SZ) {
      if (i >= dBufSize) {
	dBufSize += BUF_STEP;
	wordIn = (char *)realloc(wordIn, sizeof(char) * dBufSize);
      }
      wordIn[i++] = c;
    }

    if (i) {
      wordIn[i] = '\0';
      tR = addSequence(tR, wordIn);
    }

    free(wordIn);
  }

  tR = destroyTrie(tR);

  fclose(ifp);
  return 0;
}
