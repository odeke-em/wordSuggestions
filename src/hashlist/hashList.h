// Author: Emmanuel Odeke <odeke@ualberta.ca>
#ifndef _HASHLIST_H
#define _HASHLIST_H

  #define INIT_HASH_LIST_SIZE 100000
  typedef int hashValue;

  typedef enum {
    False=0, True=1
  } Bool;

  typedef struct Element_ {
    void *value;
    struct Element_ *next;
  } Element;

  typedef struct {
    int capacity, size;
    Element **list;
  } HashList;

  // Returns True if the element's next entry is non-NULL
  inline Bool hasNext(Element *);

  // Next attribute accessor
  inline Element *getNext(Element *);
  inline int getSize(HashList *hl);

  Element *initElement(Element *);
  HashList *initHashListWithSize(HashList *hl, const int size);
  HashList *initHashList(HashList *);

  Element *addToHead(Element *sl, void *data);
  Element *addToTail(Element *sl, void *data, const Bool overWriteOnDup);

  Element **get(HashList *hl, hashValue hashCode);
  Element *pop(HashList *hM, const hashValue hashCode);

  // Returns the number of values freed
  long int destroySList(Element *sl);

  // Returns the number of values freed
  long int destroyHashList(HashList *hl);

  // Miscellaneous
  hashValue pjwCharHash(const char *srcW);
#endif
