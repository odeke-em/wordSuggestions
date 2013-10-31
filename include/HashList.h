// Author: Emmanuel Odeke <odeke@ualberta.ca>

#ifndef _HASHLIST_H
#define _HASHLIST_H
  #include <stdio.h>
  #include <unistd.h>

  #define INVALID_HASH_VALUE 0

  inline void raiseWarning(const char *msg) {
    if (msg != NULL) {
      fprintf(stderr, "\033[31m%s\033[00m\n", msg);
    }
  }

  typedef unsigned int uint32;
  typedef unsigned long int uint64;
  typedef uint32 hash_t;

  typedef enum {
    LT=-1, EQ=0, GT=1
  } Comparison;

  typedef struct HashElem_ {
    void *key;
    void *value;
    hash_t hashValue;
    struct HashElem_ *next;
  } HashElem;

  typedef struct {
    void * (*copyKey)(const void *);
    void * (*copyValue)(const void *);
    int (*destroyKey)(void *);
    int (*destroyValue)(void *);
    Comparison (*keyComp)(const void*, const void*);
  } KeyValueFunctor;
    
  typedef struct {
    int size;
    int currentIndex;
    KeyValueFunctor *kvFunctor;
    HashElem **elemList;
    Comparison (*compare)(const HashElem*, const HashElem*);
    int (*freeHashElem)(HashElem *, const KeyValueFunctor *);
  } HashList;

  HashList *createHashList(const uint32 size);

  int destroyHashList(HashList *list);

  // Interns/creates a hash of content in src and stores the value in
  // the hash list
  void *addHashElem(HashList *list, const char *src);

  void initHashList(HashList *list);

  hash_t genHashForList(const HashList *, const char *);

  HashElem *createElem(const void *key, const void *value, const KeyValueFunctor *kvCopier);

  void *copyStr(const void *data);

  int addEntry(HashList *storageTable, void *key, void *value);

  int freeHashElem(HashElem *elem, const KeyValueFunctor *kvFunct);
#endif
