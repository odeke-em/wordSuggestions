// Author: Emmanuel Odeke <odeke@ualberta.ca>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "errors.h"

#include "list.h"
#define DEMO_CIRCULARITY

inline void *getData(const Node *n) { 
  return n == NULL ? NULL : n->data; 
}

inline Tag getTag(const Node *n) {
  return n == NULL ? Unknown : n->tag;
}

inline void *peek(const List *l) {
  return isEmpty(l) ? NULL : getData(l->head);
}

inline int isEmpty(const List *l) {
  return (l== NULL || l->head == NULL || l->size == 0) ? 1 : 0;
}

inline void *getNextNode(const Node *n) { 
  return n == NULL ? NULL : n->next; 
}

void swap(void **a, void **b) {
  void *tmp = *a;
  *a = *b;
  *b = tmp;
}

inline unsigned int getListSize(const List *l) { 
  return l == NULL ? 0 : l->size; 
}

inline Node *allocNode(void) { 
  return (Node *)malloc(sizeof(Node)); 
}

void *listPop(Node **n) {
  void *listPopd = NULL;
  if (n != NULL && *n != NULL) {
    listPopd = (*n)->data;
    Node *nextTmp = (*n)->next; 
    free(*n);
    *n = nextTmp;
  }

  return listPopd;
}

Comparison intPtrComp(const void *i1, const void *i2) {
  if (i1 == NULL && i2 == NULL) return Equal;
  else if (i1 != NULL && i2 != NULL) {
    const int *localI1 = (int *)i1;
    const int *localI2 = (int *)i2;
    if (*localI1 != *localI2) return *localI1 < *localI2 ? Less : Greater;
    else return Equal;
  }

  return i1 == NULL ? Less : Greater;
}

Node *initNode(Node *n) {
  if (n != NULL) {
    n->tag = 0;
    n->data = NULL;
    n->next = NULL;
  }

  return n;
}

Node *createNewNode(void) {
  Node *newNode = allocNode();
  assert(newNode);
  
  newNode = initNode(newNode);
  return newNode;
}

List *initList(List *l) {
  if (l != NULL) {
    l->size = 0;
    l->head = NULL;
    l->tail = NULL;
  }

  return l;
}

List *createNewList(void) {
  List *newList = allocList();
  assert(newList);
  
  newList = initList(newList);
  return newList;
}


inline List *allocList(void) {
  return (List *)malloc(sizeof(List));
}

List *append(List *l, void *data) {
  return appendAndTag(l, data, Heapd, NULL);
}

List *appendWithFreer(List *l, void *data, void (*freer)(void *)) {
  return appendAndTag(l, data, Heapd, freer);
}

List *appendAndTag(List *l, void *data, Tag tag, void (*freer)(void *)) {
  if (l == NULL) {
    l = createNewList();
  }

  if (l->head == NULL) {
    // First item being added to the list
    l->head = createNewNode();
    l->head->data = data;
    l->head->tag = tag;
    l->tail = NULL;
    l->head->next = l->tail;
  } else if (l->tail == NULL) {
    l->tail = createNewNode(); 
    l->tail->data = data;
    l->tail->tag = tag;
    l->head->next = l->tail;
  } else {
    l->tail->next = createNewNode();
    l->tail = l->tail->next;
    l->tail->tag = tag;
    l->tail->data = data;
  }

  ++l->size;
  return l;
}

List *prepend(List *l, void *data) {
  return prependAndTag(l, data, Heapd);
}

List *prependAndTag(List *l, void *data, Tag tag) {
#ifdef DEBUG
  printf("\033[32m%s\033[00m\n", __func__);
#endif

  if (l == NULL) {
    l = createNewList();
  }

  if (l->head == NULL) {
    // First item being added to the list
    l->head = createNewNode();
    l->head->data = data;
    l->head->tag = tag;
    l->tail = l->head;
  } else {
    // Adding to the front
    Node *newEnd = createNewNode();
    newEnd->data = data;
    newEnd->next = l->head;
    newEnd->tag = tag;
    l->head = newEnd;
    l->tail->next = l->head;
  }

  ++l->size;

  return l;
}

int freeFromHeadToTail(Node *head, Node *tail) {
  int freeCount = 0;

  return freeCount;
}

void destroyList(List *l) {
  if (l != NULL) {
    Node *start = l->head, *end = l->tail, *tmp = NULL;

    if (start != NULL) {
      void (*dataFreer)(void *) = NULL;
      while (start != end) {
	tmp = start->next;
	if (start == NULL) break;

	if (start->tag == Heapd && start->data != NULL)  {
	  if (start->freeData == NULL) 
	    free(start->data);
	  else
	    start->freeData(start->data);
	}

	free(start);
	start = tmp;
      }

      if (end != NULL) {
	if (end->data != NULL) free(end->data);
	free(end);
      }
    }

    l->tail = NULL; // No more access to the tail either

    free(l);
    l = NULL;
  }
}

void printList(List *l) {
  printf("[");
  if (l != NULL) {
    Node *it = l->head, *end = l->tail;
    while (it != NULL) {
      if (it->data  != NULL) printf(" %d:%d ", *(int *)it->data, it->tag);
      if (it == end) break;
      it = it->next;
    }
  }
  printf("]");
}

List *multiMerge(unsigned int count, ...) {
  // Pass the number of lists to merge, the lists themselves
  // and the very last argument should be the Comparator
  List *merged = NULL;
#ifdef DEBUG
  printf("Func:: %s\n", __func__);
#endif

  va_list ap;
  va_start(ap, count);

  List **lBlock = (List **)malloc(sizeof(List *) * count);
  List *tmpL = NULL;

  int i=0;
  while (i < count) {
    tmpL = va_arg(ap, List *);
    lBlock[i++] = tmpL;
  }

  Comparator matchFunc = va_arg(ap, Comparator);

#ifdef DEBUG
  printf("ap: %p\n", ap);
#endif

  va_end(ap);
  if (i) {
    int popularCount = count, midIndex = i/2, maxCount = i, gallop;
    int thresholdCount = 0.75 * count;
    List **minList = NULL;
    while (popularCount >= thresholdCount) {
    #ifdef DEBUG
      for (i=0; i < maxCount; ++i) {
	printList(lBlock[i]); 
	printf("\n");
      }
      printf("\033[35m\nBlockEntries\n\033[00m");
    #endif

      popularCount = maxCount;
      pickMinList: {
	int minIdx=0;
	while (minIdx < maxCount) {
	  minList = lBlock + minIdx;
	  if (peek(*minList) != NULL) {
	    break;
	  } else {
	    ++minIdx;
	  }
	}

	if (peek(*minList) == NULL) {
	  printf("No more selections for minList can be made\n");
	  goto cleanUpForReturn;
	}
      }

      int gallop = 0, nonEmptyCount = 0;
      for (i=0; i < maxCount; ++i) {
	if (peek(lBlock[i]) == NULL) {
	  --popularCount;
	} else {
	  ++nonEmptyCount;
	  Comparison comp = matchFunc(peek(*minList), peek(lBlock[i]));
	  if (comp == Equal) {
	    ++gallop;
	  } else if (comp == Greater) {
	    minList = lBlock + i;
	  }
	}
      }

      if (gallop == nonEmptyCount) { // All elements were equal
	for (i=0; i < maxCount; ++i) {
	  if (peek(lBlock[i]) != NULL) {
	    merged = append(merged, popHead(lBlock[i]));
	  }
	}
      } else {
	merged = append(merged, popHead(*minList));
      }
    }

    for (i=0; i < maxCount; ++i) {
      while (peek(lBlock[i]) != NULL) {
	merged = append(merged, popHead(lBlock[i]));
      }
    }
  }

  cleanUpForReturn: {
    free(lBlock);
  }
  
  return merged;
}

Node *find(List *l, void *query, Comparator matchFunc) {
  Node *result = NULL;
  if (l != NULL && matchFunc != NULL) {
    Node *it = l->head, *end = l->tail;
    do {
      if (it == NULL) break;
    
      if (matchFunc(it->data, query) == Equal) {
        result = it;
        break;
      }
      it = it->next;
    } while (it != end);
  }

  return result;
}

void *popHead(List *l) {
  void *popd = NULL;
  if (l != NULL && l->head != NULL) {
    popd = listPop(&l->head);
    --l->size;
  }

  return popd;
}

List *removeElem(List *l, void *query, Comparator matchFunc) {
  if (l != NULL && matchFunc != NULL) {
    Node *prev = NULL, *cur = l->head, *end = l->tail;
    while (cur != NULL) {
      if (matchFunc(cur->data, query) == Equal) break;

      prev = cur;
      cur = cur->next;
      if (cur == end) break;
    }

    if (cur != NULL) {
      if (prev != NULL) {
	prev->next = cur->next;
      } else {
	l->head = prev = cur->next;
      }
	
      if (cur->data != NULL) free(cur->data);
      free(cur);
      cur = NULL;
      --l->size;
    }
  }
  return l;
}


#ifdef SAMPLE_RUN
int main() {
  int i;
  List *l = NULL;

  List *(* listAddFunc)(List *, void *) = append;
#ifdef DEMO_PREPEND
  listAddFunc = prepend;
#endif

  for (i=0; i < 40; ++i) {
    int *tp = (int *)malloc(sizeof(int));
    *tp = i;
    l = listAddFunc(l, tp);
  }

#ifdef DEMO_CIRCULARITY
  while (l->head != NULL) {
    void *vSav = listPop(&(l->head));
    if (vSav != NULL) {
      printf("vS: %d\n", *(int *)vSav);
      free(vSav);
    }
    break;
  }
#endif

  printList(l);
  printf("\n");

  i = 8;

  printf("Size: %d\n", getListSize(l));
  l = removeElem(l, &i, intPtrComp);

  printf("Size: %d\n", getListSize(l));
  printList(l);
  printf("\n");
  destroyList(l);
  return 0;
}
#endif
