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
    int MAX_SZ = 50;
    while (! feof(ifp)) {
      int bufSiz = 10, index = 0;
      char c;
      char *wIn = (char *)malloc(sizeof(char) * bufSiz);
      while ((c = getc(ifp)) && isalnum(c) && index < MAX_SZ) {
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
  return destroyTrieAndPayLoads(tr, NULL);
}

Trie *destroyTrieAndPayLoads(Trie *tr, void *(*pLoadFreer)(void *)) {
#ifdef DEBUG
  printf("In %s\n", __func__);
#endif
  if (tr == NULL) return tr;

  if (tr->keys != NULL) {
    Trie **it = tr->keys;
    Trie **end = tr->keys + tr->radixSz;
    while (it < end) {
      if (*it != NULL) {
	*it = destroyTrieAndPayLoads(*it, pLoadFreer);
      }
      ++it;
    }

    if (tr->loadTag == HeapD) { // Memory from the heap
      if (tr->payLoad != NULL && pLoadFreer != NULL) {
	pLoadFreer(tr->payLoad);
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

void exploreTrie(const Trie *t, const char *pAxiom, FILE *ofp) {
  if (t != NULL) {
    if (t->keys != NULL) {
      Trie **start = t->keys, 
	   **end = start + t->radixSz, 
	   **it;
      ssize_t pAxiomLen = strlen(pAxiom);
      for (it = start; it < end; ++it) {
	if (*it != NULL) {
	  char *ownAxiom = (char *)malloc(pAxiomLen + 2); // Space for own len
	  memcpy(ownAxiom, pAxiom, pAxiomLen);
          ownAxiom[pAxiomLen] = (it - start) + alphaStart; //Own index
	  ownAxiom[pAxiomLen + 1] = '\0'; // NULL terminate this one as well
	  if ((*it)->EOS) {
	    fprintf(ofp, "%s\n", ownAxiom);
	  }
	  exploreTrie(*it, ownAxiom, ofp);
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
      tr->loadTag = tag;
    }
  }

  return tr;
}

Trie *addSequence(Trie *tr, const char *seq) {
  return addSequenceWithLoad(tr, seq, NULL, StackD);
}

int searchTrie(const Trie *tr, const char *seq, void **ptrSav) {
  if (seq == NULL || tr == NULL || tr->keys == NULL) {
    return -1;
  }

  if (*seq == '\0') {
    if (ptrSav != NULL) {
      *ptrSav = tr->payLoad;
    }

    return 1;
  }

  int resIndex = resolveIndex(*seq);
  if (resIndex < 0 || resIndex >= radixSize) {
    return -1; 
  } 
  if (tr->keys[resIndex] == NULL) {
    return 0;
  } else {
    return searchTrie(tr->keys[resIndex], seq+1, ptrSav);
  }
}

Trie *allocTrie() { 
  return (Trie *)malloc(sizeof(Trie)); 
}

int resolveIndex(const char c) {
  int resIndex = -1;
  if (isalpha(c)) {
    resIndex = c-alphaStart;
  } else if (isspace(c)) {
    resIndex = (alphaDiff + 1);
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
  void *found = NULL;
  int iQuery = searchTrie(tR, "mb\0", &found);
  printf("\033[%dmFound: %d\033[00m\n", iQuery == -1 ? 31 : 33, iQuery);

  // Consume self
  FILE *ifp = fopen(__FILE__, "r");
  int BUF_STEP = 10, MAX_SINGLE_ALLOC_SZ = 60;
  char c;

  Trie *fTrie = trieFromFile(ifp);
  fTrie = addSequenceWithLoad(fTrie, "mbc\0", "flux\0", StackD);
  exploreTrie(fTrie, "", stdout);
  iQuery = searchTrie(fTrie, "mbc\0", &found);
  printf(
    "\033[%dmFound: %d ptr: %p\033[00m\n", iQuery == -1 ? 31: 33, iQuery, found
  );
  fTrie = destroyTrie(fTrie);
  tR = destroyTrie(tR);

  fclose(ifp);
  return 0;
}
#endif
