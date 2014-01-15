// Author: Emmanuel Odeke <odeke@ualberta.ca>
#include <stdio.h>
#include <stdlib.h>

#include "LRU.h"

#define CLEAR_TAGS_AFTER_PURGE

Cache *setTagValue(Cache *c, unsigned int tagValue) {
  if (c != NULL) {
    Node *start = c->head, *end = c->tail;
    while (start != NULL) {
      start->tag = tagValue;
      if (start == end) break;
      start = start->next;
    }
  }

  return c;
}

Cache *purgeLRU(Cache *c) {
  return purgeAndSave(c, NULL);
}

Cache *purgeAndSave(Cache *c, Cache **purgedSav) {
  if (c != NULL) {
    Node *it = c->head, *end = c->tail, *prev = c->head;
    while (it != NULL) {
      if (it->tag == 0) { // Hasn't been accessed since the last cycle
   
	if (it->data != NULL) {
	#ifdef DEBUG
	  printf("Purging: %d\n", *(int *)it->data);
	#endif
	  --c->size;
          if (purgedSav != NULL) { 
	    *purgedSav = append(*purgedSav, it->data);
          } else {
            free(it->data);
          }
	  it->data = NULL;
	}

	if (prev == NULL) {
	  c->head = prev = it->next;
	  free(it);
	  it = prev;
	} else {
	  prev = it->next;
	  it = it->next;
	}
      } else {
      #ifdef DEBUG
	printf("Couldn't purge: %d\n", *(int *)it->data);
      #endif
	prev = it;
	it = it->next;
      }

      if (it == end) break;
    }
  }

#ifdef CLEAR_TAGS_AFTER_PURGE
  c = setTagValue(c, 0); // Now elements have to be reaccessed in next cycle
#endif
  return c;
}

void *lookUpEntry(Cache *c, void *key, Comparator comp) {
  if (c != NULL) {
    Node *queryNode = find(c, key, comp);
    if (queryNode != NULL) {
      queryNode->tag = 1;
      return queryNode->data;
    }
  }

  return NULL;
}

#ifdef SAMPLE_LRU
int main() {
  Cache *c = NULL;
  int i=0;
  for (i=0; i < 10; ++i) {
    int *newI = (int *)malloc(sizeof(int));
    *newI = i;
    c = prepend(c, newI);
  }

  c = setTagValue(c, 0);
  printList(c);
  printf("\n");

  printf("Before purge 1\n");
  printList(c);

  for (i= 2; i < 8; ++i) {
    lookUpEntry(c, &i, intPtrComp);
  }

  printf("\nAfter lookUp 1\n");
  printList(c);

  printf("\nAfter purge 1\n");
  c = purgeLRU(c);
  printList(c);
  printf("\n");
  printf("Next cycle\n");

  for (i=0; i < 15; ++i) {
    int *newI = (int *)malloc(sizeof(int));
    *newI = i;
    c = append(c, newI);
  }

  for (i=90; i < 100; ++i) {
    lookUpEntry(c, &i, intPtrComp);
  }

  printf("After second set of accesses\n");
  printList(c);

  Cache *purgeSav=NULL;
  c = purgeAndSave(c, &purgeSav);
  printf("\nAfter purge 2:: Fresh list\n");
  printList(c);
  printf("\n");
  printf("Purged and saved\n");
  printList(purgeSav);
  printf("\n");

  destroyList(purgeSav);
  destroyList(c);
  return 0;
}
#endif
