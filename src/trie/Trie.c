// Author: Emmanuel Odeke <odeke@ualberta.ca>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Trie.h"
#include "errors.h"

#ifdef DEBUG
  #undef DEBUG
#endif


Trie *createTrie() {
  Trie *freshTrie = allocTrie();
  if (freshTrie == NULL) {
    raiseError("Run-out of memory");
  }

  freshTrie->EOS = 0;
  freshTrie->loadTag = StackD;
  freshTrie->payLoad = NULL;
  freshTrie->radixSz = radixSize;

  freshTrie->keys = (Trie **)malloc(sizeof(Trie *) * radixSize);

  if (freshTrie->keys == NULL) {
    raiseError("Run-out of memory");
  }

  Trie **it = freshTrie->keys,
       **end= it + freshTrie->radixSz;

  while (it != end) {
    *it++ = NULL;
  }

  return freshTrie;
}

Trie *trieFromFile(FILE *ifp) {
  Trie *fTrie = NULL;
  if (ifp != NULL) {
    fTrie = createTrie();
    while (! feof(ifp)) {
      int bufSiz = 10, index = 0;
      char c;
      char *wIn = (char *)malloc(sizeof(char) * bufSiz);
      while ((c = getc(ifp)) && isalnum(c)) {
	if (index >= bufSiz) {
	  bufSiz += 10; // Arbitrary increase of 10
          wIn = (char *)realloc(wIn, sizeof(char) * bufSiz);
	}
	
	wIn[index++] = c;
      }

      if (index) {
	wIn[index++] = '\0';
	wIn = (char *)realloc(wIn, sizeof(char) * index);
	fTrie = addSequence(fTrie, wIn);
      }

      free(wIn);
    }
  }

  return fTrie;
}

Trie *destroyTrie(Trie *tr) {
#ifdef DEBUG
  printf("In %s\n", __func__);
#endif
  if (tr == NULL) return tr;

  if (tr->keys != NULL) {
    Trie **it = tr->keys;
    Trie **end = tr->keys + tr->radixSz;
    while (it < end) {
      if (*it != NULL) {
	*it = destroyTrie(*it);
      }
      ++it;
    }

    if (tr->loadTag == HeapD) { // Memory from the heap
      if (tr->payLoad != NULL) {
	free(tr->payLoad);
	tr->payLoad = NULL;
      }
    }
    free(tr->keys);
    tr->keys = NULL;
  }


  free(tr);
  tr = NULL;
  return tr;
}

void exploreTrie(Trie *t, const char *pAxiom) {
  if (t != NULL) {
    if (t->keys != NULL) {
      Trie **start = t->keys, 
	   **end = start + t->radixSz, 
	   **it;
      ssize_t pAxiomLen = strlen(pAxiom);
      for (it = start; it != end; ++it) {
	if (*it != NULL) {
	  char *ownAxiom = (char *)malloc(pAxiomLen + 2); // Space for own len
	  memcpy(ownAxiom, pAxiom, pAxiomLen);
          ownAxiom[pAxiomLen] = (it - start) + radixStart; //Own index
	  ownAxiom[pAxiomLen + 1] = '\0'; // NULL terminate this one as well
	  if ((*it)->EOS) {
	    printf("%s\n", ownAxiom);
	  }
	  exploreTrie(*it, ownAxiom);
	  free(ownAxiom);
	}
      }
    }
  }
}

Trie *addSequenceWithLoad(
  Trie *tr, const char *seq, void *payLoad, const TrieTag tag
) {
#ifdef DEBUG
  printf("%s seq: %s\n", __func__, seq);
#endif
  if (tr == NULL || tr->keys == NULL) {
    raiseError("Cannot add elements to a NULL Trie");
  }

  if (seq != NULL) {
    if (*seq != '\0') {
      int targetIndex = resolveIndex(*seq);
      if (targetIndex >= 0 && targetIndex < radixSize) {
	if (tr->keys[targetIndex] == NULL) {
	  tr->keys[targetIndex] = createTrie();
	#ifdef DEBUG
	  printf("New Trie alloc index: %d\n", targetIndex);
	#endif
	}

	tr->keys[targetIndex] =\
	   addSequenceWithLoad(tr->keys[targetIndex], seq+1, payLoad, tag);
      }
    } else { // End of this sequence, time to deploy the payLoad
      tr->EOS = 1;
      tr->payLoad = payLoad;
    }
  }

  return tr;
}

Trie *addSequence(Trie *tr, const char *seq) {
  return addSequenceWithLoad(tr, seq, NULL, StackD);
}

void *searchTrie(Trie *tr, const char *seq) {
  if (seq == NULL || tr == NULL || tr->keys == NULL) {
    return 0;
  }

  if (*seq == '\0') return tr->payLoad;

  int resIndex = resolveIndex(*seq);
  if (resIndex < 0 || resIndex >= radixSize || tr->keys[resIndex] == NULL) {
    return NULL;
  } else {
    return searchTrie(tr->keys[resIndex], seq+1);
  }
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

#ifdef REV_TRIE
int main() {
  Trie *tR = createTrie();
  tR = addSequence(tR, "abc\0");
  tR = addSequenceWithLoad(tR, "mbc\0", "flux\0", StackD);
  tR = addSequenceWithLoad(tR, "mb\0", "fox\0", StackD);
  tR = addSequence(tR, "mac\0");
  void *found = searchTrie(tR, "mb\0");
  printf("\033[%dmFound: %p\033[00m\n", found == NULL ? 31 : 33, found);

  // Consume self
  FILE *ifp = fopen(__FILE__, "r");
  int BUF_STEP = 10, MAX_SINGLE_ALLOC_SZ = 60;
  char c;

  Trie *fTrie = trieFromFile(ifp);
  fTrie = addSequenceWithLoad(fTrie, "mbc\0", "flux\0", StackD);
  exploreTrie(fTrie, "");
  found = searchTrie(fTrie, "mbc\0");
  printf("\033[%dmfTrieFound: %p\033[00m\n", found == NULL ? 31 : 33, found);
  fTrie = destroyTrie(fTrie);
  tR = destroyTrie(tR);

  fclose(ifp);
  return 0;
}
#endif
