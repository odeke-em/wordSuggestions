// Author: Emmanuel Odeke <odeke@ualberta.ca>
#include <stdio.h>
#include <stdlib.h>

#include "list.h"
#define CLEAR_TAGS_AFTER_PURGE

typedef List Cache;
Cache *setTagValue(Cache *c, unsigned int tagValue) {
  if (c != NULL) {
    Node *start = c->head, *end = c->tail;
    do {
      if (start == NULL) break;
      start->tag = tagValue;
      start = start->next;
    } while (start != end);
  }

  return c;
}

Cache *purgeLRU(Cache *c) {
  if (c != NULL) {
    Node *it = c->head, *end = c->tail, *prev = c->head;
    while (it != NULL) {
      if (it->tag == 0) { // Hasn't been accessed since the last cycle
   
	if (it->data != NULL) {
	#ifdef DEBUG
	  printf("Purging: %d\n", *(int *)it->data);
	#endif
	  free(it->data);
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
    }
  }

#ifdef CLEAR_TAGS_AFTER_PURGE
  c = setTagValue(c, 0); // Now elements have to be reaccessed in next cycle
#endif
  return c;
}

Cache *accessMember(Cache *c, void *entry, Comparator comp) {
  if (c != NULL) {
    Node *entryNode = find(c, entry, comp);
    if (entryNode != NULL) 
      entryNode->tag = 1;
  }

  return c;
}

int main() {
  Cache *c = NULL;
  int i=0;
  for (i=0; i < 10; ++i) {
    int *newI = (int *)malloc(sizeof(int));
    *newI = i;
    c = append(c, newI);
  }

  printList(c);
  c = setTagValue(c, 0);
  printf("\n");

  for (i= 2; i < 8; ++i) {
    c = accessMember(c, &i, intPtrComp);
  }

  printf("Before purge 1\n");
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

  for (i=0; i < 10; ++i) {
    c = accessMember(c, &i, intPtrComp);
  }

  printf("After second set of accesses\n");
  printList(c);

  c = purgeLRU(c);
  printf("\nAfter purge 2\n");
  printList(c);
  printf("\n");

  destroyList(c);
  return 0;
}
