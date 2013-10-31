// Author: Emmanuel Odeke <odeke@ualberta.ca>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/HashList.h"

int mallocCall=0;
int deallocCall=0;

inline void *allocMgr(const ssize_t size) {
  ++mallocCall;

#ifdef DEBUG
  printf("\nMallocing %d\n", mallocCall);
#endif

  return malloc(size);
}

inline void *deAllocMgr(void *data) {
  if (data == NULL) return;
  ++deallocCall;

#ifdef DEBUG
  printf("\nDeallocation Count: %d\n", deallocCall);
#endif

  free(data);
}

HashList *createHashList(const uint32 size) {
  HashList *newHL = NULL;
  if ((newHL = (HashList*)allocMgr(sizeof(HashList))) == NULL) {
    return NULL;
  }

  if ((newHL->elemList = (HashElem **)allocMgr(sizeof(HashElem *) * size)) == NULL) {
    deAllocMgr(newHL);
    return NULL;
  }

  newHL->size = size;
  newHL->currentIndex = 0;
  newHL->kvFunctor =  NULL;
  newHL->freeHashElem = NULL;
  newHL->compare = NULL;
 
  //Initialize each hashElement
  int i;
  for (i=0; i < newHL->size; ++i) {
    newHL->elemList[i] = NULL; //initHashElem(newHL->elemList[i]);
  }

#ifdef DEBUG
  printf("\nallocated hashList of size %d\n", newHL->size);
#endif

  return newHL;
}

int destroyHashList(HashList *list) {
  if (list != NULL && list->elemList != NULL) {
    if (list->kvFunctor == NULL) {
      // Report that the functStruct is NULL
      raiseWarning(
	"Could not use the function storage struct in destroyHashList"
      );
      return -1;
    }

    int (*destroyElem)(HashElem *, const KeyValueFunctor *) = list->freeHashElem;
    if (destroyElem == NULL) {
      raiseWarning("NULL destroy function passed in");
      return -1;
    }

    int freedCount = 0, i;

    for (i=0; i < list->size; ++i) {
      freedCount += destroyElem(list->elemList[i], list->kvFunctor);
    }

    deAllocMgr(list->elemList);
    deAllocMgr(list);
    list = NULL;

    return freedCount;
  }

  return -1;
}

hash_t generateHash(const HashList *targetTable, const char *str) {
  if (targetTable == NULL || str == NULL) {
    return INVALID_HASH_VALUE;
  }

  uint64 hashVal = 0;
  size_t keyLen = strlen(str);

  int i = 0;

  // Hashing function copied from a github gist
  while (i < keyLen) {
    hashVal <<= 2;
    hashVal += str[i];

    ++i;
  }

  return hashVal % targetTable->size;
}

HashElem *createElem(const void *key, const void *value, const KeyValueFunctor *kvFunct) {
  if (key == NULL) {
    raiseWarning("NULL keys not accepted");
    return NULL;
  }

  if (kvFunct == NULL || kvFunct->copyValue == NULL || kvFunct->copyKey == NULL) {
    raiseWarning("Functions to copy both key and value must not be NULL");
    return NULL;
  }

  HashElem *elem = (HashElem *)allocMgr(sizeof(HashElem));
  elem->key   = kvFunct->copyKey(key);
  elem->value = kvFunct->copyValue(value);

  elem->next = NULL;
  return elem;
}

Comparison strComp(const void *st1, const void *st2) {
  if (st1 == NULL) return LT;
  else if (st2 == NULL) return GT;

  const char *s1 = (char *)st1;
  const char *s2 = (char *)st2;

  return strcmp(s1, s2);
}

int addEntry(HashList *storageTable, void *key, void *value) {
  if (storageTable == NULL) {
    raiseWarning("Cannot add elements to empty hashList");
    return -1;
  }

  if (key == NULL) {
  #ifdef DEBUG
    raiseWarning("Cannot add an empty key");
  #endif

    return -1;
  }

  if (storageTable->kvFunctor == NULL) {
    raiseWarning(
      "Expecting a non NULL keyValue storage comparator for keys to addEntries"
    );
    return -1;
  }

  Comparison (*keyComparator)(const void *, const void *) = storageTable->kvFunctor->keyComp;

#ifdef DEBUG
  printf("Comparison is NULL %d\n", keyComparator == NULL);
#endif

  if (keyComparator == NULL) {
    raiseWarning(
      "A non NULL comparator is needed in the storageTable for adding entries"
    );
    return -1;
  }

  hash_t qHashIndex = generateHash(storageTable, (char *)key);

#ifdef DEBUG
  printf("\nqHashIndex %u\n", qHashIndex);
#endif

  //Time to handle collisions
  HashElem *foundBucket = storageTable->elemList[qHashIndex];
  HashElem *last = NULL;

  while (foundBucket != NULL) {
    if (keyComparator(foundBucket->key, key) == EQ) { //Update that key's value with the latest
      deAllocMgr(foundBucket->value);
      foundBucket->value= copyStr(value);

      return 1;
    }

    last = foundBucket;
    foundBucket = foundBucket->next;
  }

  HashElem *newElem = createElem(key, value, storageTable->kvFunctor);

  // Key not found, time to add it to the list
  if (foundBucket == storageTable->elemList[qHashIndex]) { // At the start of a bucket
    storageTable->elemList[qHashIndex] = newElem;
  } else if (foundBucket == NULL) {
    last->next = newElem;
  } else { // Add the newElem to the head of the bucket
    newElem->next = foundBucket;
    last->next = newElem;
  } 

  return 0;
}

void *get(HashList *srcList, const void *queryKey) {
  if (srcList == NULL || queryKey == NULL) return NULL;

  hash_t queryHash =  generateHash(srcList, (char *)queryKey);

  HashElem *retrieved = srcList->elemList[queryHash];
  Comparison (*comp)(const void*, const void*) = srcList->kvFunctor->keyComp;

  while (retrieved != NULL) {
    if (comp(retrieved->key, queryKey) == EQ) return retrieved->key;

    retrieved = retrieved->next;
  }

  return NULL;
}

void *copyStr(const void *data) {
  if (data == NULL) return NULL;

  return (void *)strdup((char *)data);
}

void printElem(const HashElem *elem) {
  if (elem != NULL) {
    printf( " %s : %s ", elem->key == NULL ? "?": (char *)elem->key, 
		       elem->value == NULL ? "?":  (char *)elem->value
    );
    printElem(elem->next);
  }

}

int destroyHashElem(HashElem *elem, const KeyValueFunctor *kvFunct) {
  if (kvFunct == NULL || kvFunct->destroyKey == NULL || kvFunct->destroyValue == NULL) {
    raiseWarning("KeyValue destroy functions cannot be NULL");
    return;
  }

  if (elem == NULL) {
    return 0;
  } else {
    int destroyedCount = 0;
    destroyedCount += kvFunct->destroyKey(elem->key);
    destroyedCount += kvFunct->destroyValue(elem->value);

    //Saving a copy of the next item
    HashElem *next = elem->next;

    deAllocMgr(elem);
    elem = next;
    return destroyedCount + destroyHashElem(elem, kvFunct);
  }
}

int freeStr(void *data) {
  if (data == NULL) return 0;

  deAllocMgr(data);
  data = NULL;
  return 1;
}

void initHashElem(HashElem *elem) {
  if (elem == NULL) return;

  elem->key = elem->value = NULL;
  elem->next = NULL;
}

int freeHashElem(HashElem *elem, const KeyValueFunctor *kvFunct) {
  if (elem == NULL) return 0;
  else if (kvFunct == NULL || kvFunct->destroyKey == NULL || kvFunct->destroyValue == NULL) {
    raiseWarning("Non NULL key and value destroyer functions needed");
    return -1;
  }

  int nDestroys = kvFunct->destroyKey(elem->key);
  nDestroys += kvFunct->destroyValue(elem->value);
  deAllocMgr(elem);
  elem = NULL;

  ++nDestroys; // An increment for the destruction of the storage element itself

  return nDestroys;
}

HashElem *genericHashElem(const void *key, const void *value) {
  KeyValueFunctor kvFunct;
  kvFunct.copyKey = copyStr;
  kvFunct.copyValue = copyStr;
  kvFunct.destroyKey = freeStr;
  kvFunct.destroyValue = freeStr;

  return createElem(key, value, &kvFunct);
}

void printList(HashList *ht) {
  if (ht == NULL || ht->elemList == NULL) return;
  int i;

  for (i=0; i<ht->size; ++i) {
    printElem(ht->elemList[i]);
  }
}

#ifdef SAMPLE_RUN
int main() {
  char *s = "odEKE\0";
  char *p = "ODEKA\0";
  char *r = "emmanuel\0";

  KeyValueFunctor kvFunct;
  kvFunct.copyKey = copyStr;
  kvFunct.copyValue = copyStr;
  kvFunct.destroyKey = freeStr;
  kvFunct.destroyValue = freeStr;
  kvFunct.keyComp = strComp;

  HashElem *elem = createElem(s, r, &kvFunct);
  printElem(elem);

  HashList *ht = createHashList(2000);
  ht->kvFunctor = &kvFunct;

  ht->freeHashElem = destroyHashElem;

  int addResult = addEntry(ht, s, NULL);
  addResult = addEntry(ht, p, s);
  addResult = addEntry(ht, r, s);
  addResult = addEntry(ht, p, s);
  addResult = addEntry(ht, p, s);

  int destroyed = destroyHashElem(elem, &kvFunct);

  printf("\naddResult %d destroyed %d\n", addResult, destroyed);
  printf("Size: %d\n", ht->size);
  printList(ht);
  addResult += destroyHashList(ht);

  return 0;
}
#endif
