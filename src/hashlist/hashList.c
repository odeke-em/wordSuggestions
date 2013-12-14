// Author: Emmanuel Odeke <odeke@ualberta.ca>
// HashList implementation

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "errors.h"
#include "hashList.h"

#ifdef DEBUG
  #undef DEBUG
#endif

#define HANDLE_COLLISIONS
#define EXHIBIT_COLLISION
#define EXHIBIT_GET_BY_REFERENCE

inline Bool hasNext(Element *e) { return e != NULL && e->next != NULL; }
inline Element *getNext(Element *e) { return e == NULL ? NULL : e->next; }
inline int getSize(HashList *hl) { return hl == NULL ? 0 : hl->size; }

Element *addToTail(Element *sl, void *data, const Bool overWriteOnDup) {
  if (sl == NULL) {
    sl = initElement(sl);
    sl->value = data;
  } else if (sl->value != data) {
    sl->next = addToTail(sl->next, data, overWriteOnDup);
  } else {
    if (overWriteOnDup) {
      sl->value = data;
    } else {
      //Do something interesting eg store number of visits
    }
  }

  return sl;
}

Element *addToHead(Element *sl, void *data) {
  if (sl != NULL) {
    sl = initElement(sl);
  }

  Element *newElem = NULL;
  newElem = initElement(newElem);
  newElem->value = data;
  newElem->next = sl;
  sl = newElem;

  return sl;
}

Element *initElement(Element *elem) {
  if (elem == NULL) {
    elem = (Element *)malloc(sizeof(Element));
  } 

  elem->next = NULL;
  elem->value = NULL;

  return elem;
}

HashList *initHashListWithSize(HashList *hl, const int size) {
  if (hl == NULL) {
    hl = (HashList *)malloc(sizeof(HashList));
    raiseExceptionIfNull(hl);
  }

  if (size > 0) {
    hl->size = size;
  #ifdef DEBUG
    printf("%s:: %d\n", __func__, hl->size);
  #endif
    if ((hl->list = (Element **)malloc(sizeof(Element *) * hl->size)) == NULL) {
      free(hl);
      raiseError(
      "Run out of memory, trying to create space for a hashlist's list attribute"     );
    }

    // All elements set to NULL
    Element **listIter = hl->list, **end = hl->list + hl->size;
    while (listIter != end) {
    #ifdef DEBUG
      printf("lIt: %p end: %p\n", listIter, end);
    #endif
      *listIter++ = NULL;
    }
  }

  return hl;
}

HashList *initHashList(HashList *hl) {
  return initHashListWithSize(hl, INIT_HASH_LIST_SIZE);
}

void insertElem(HashList *hl, void *data, const hashValue hashCode) {
  if (hl == NULL) {
    raiseError("Cannot add elements to a NULL hashList");
  } 

  if (hl->size == 0) {
    printf("HashList size is zero, initializing it now\n");
    hl = initHashList(hl);
    assert(hl->size != 0);
  }

  int elemIndex = hashCode % hl->size;
#ifdef DEBUG
  printf("HashCode: %d Element at Index: %p\n", hashCode, hl->list[elemIndex]);
#endif
  if (hl->list[elemIndex] == NULL) { // We've found the first entry matching that hash
    hl->list[elemIndex] = initElement(hl->list[elemIndex]);
    hl->list[elemIndex]->value = data;
  } else {
    #ifdef HANDLE_COLLISIONS
      // Always update to the latest value
      hl->list[elemIndex] = addToTail(hl->list[elemIndex], data, True);
    #else 
      hl->list[elemIndex]->value = data;
    #endif
  }
}

Element **get(HashList *hl, hashValue hashCode) {
  return (hl == NULL || ! hl->size || hl->list == NULL) \
	  ? NULL : &(hl->list[hashCode % hl->size]);
}

long int destroySList(Element *sl) {
  long int nValueFrees = 0;
  if (sl != NULL) { 
    // printf("Sl == NULL: %d\n", sl != NULL);
    Element *tmp;
    while (sl != NULL) { 
      tmp = sl->next;
    #ifdef DEBUG
      printf("Freeing:: curHead: %p Next: %p\n", sl, tmp);
    #endif
      if (sl->value != NULL) {
	free(sl->value);
	++nValueFrees;
      }

      free(sl);

      sl = tmp;
    }
    sl = NULL;
  }

  return nValueFrees;
}

Element *pop(HashList *hM, const hashValue hashCode) {
  Element *pElement = NULL;

  if (getSize(hM)) {
    unsigned int calcIndex = hashCode % getSize(hM);
    pElement = hM->list[calcIndex];
    hM->list[calcIndex] = NULL;
  }

#ifdef DEBUG
  printf("hPElement: %p\n", pElement);
#endif

  return pElement;
}

long int destroyHashList(HashList *hl) {
  long int nValueFrees = 0;
  if (hl != NULL) {
    int i;
    if (hl->list != NULL) {
      for (i=0; i < hl->size; ++i) {
	nValueFrees += destroySList(hl->list[i]);
      }

      free(hl->list);
      hl->list = NULL;
    }

    free(hl);
    hl = NULL;
  }

  return nValueFrees;
}

hashValue pjwCharHash(const char *srcW) {
  // PJW hashing algorithm
  hashValue h = 0;

  unsigned int g, i, 
	       srcLen = strlen(srcW) / sizeof(char);

  for (i=0; i < srcLen; ++i) {
    h = (h << 4) + srcW[i];
    g = h & 0xf0000000;
    if (g) {
      h ^= (g >> 24);
      h ^= g;
    }
  }

  return h;
}

#ifdef SAMPLE_RUN
int main() {
  HashList *hl = NULL;

  unsigned int hSize = 10000000;
#ifdef EXHIBIT_COLLISION
  hl = initHashListWithSize(hl, hSize/10);
#else
  hl = initHashListWithSize(hl, hSize);
#endif
  char *tmp = (char *)malloc(4);
  insertElem(hl, tmp, 2);

  int i;
  for (i=0; i < hSize; i++) {
    int *x = (int *)malloc(sizeof(int));
    *x = i;
    insertElem(hl, x, i);
  }

  printf("hl %p\n", hl);

#ifdef EXHIBIT_GET_REFERENCE
  Element **found = get(hl, 101);

  while (*found != NULL) {
    printf("FND: %d\n", *((int *)(*found)->value));

    Element *curHolder = *found;
    *found = (*found)->next;
     
    if (curHolder->value != NULL) free(curHolder->value); 
    free(curHolder);

  }
  if (found != NULL) free(*found);
#endif

  hashValue hTest = 101;
  Element *popd = pop(hl, hTest);

  while (popd != NULL) {
    Element *cur = popd;
    popd = getNext(popd);

    if (cur->value != NULL) {
      printf("Elem with hash: %d :: %d\n", hTest, *((int *)cur->value));
      free(cur->value);
    }

    free(cur);
  }

  destroyHashList(hl);
  return 0;
}
#endif
