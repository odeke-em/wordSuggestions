#ifndef _HASHLIB_H
#define _HASHLIB_H
  #include "customTypes.h"
  #include "errors.h"

  #define INIT_HASH_VALUE 0
  #define UNDEF_HASH_VALUE -1
  #define INIT_N_HASH_ELEMS 20

  hash hashFunc(const word);
  ElemComparison hashComp(const hash, const hash);

  void addHash(hashList *, const word);

  hashElem *allocHashElem(void);
  void initHashElem(hashElem *);

  int printHashElem(const hashElem *);

  void setHash(hashElem *, const word);

  void freeHashElem(hashElem);

  //Free hashElem pointer version
  void freeHashElemPtr(hashElem *);

  //Load up all the words from a file path into a hashlist
  hashList *fileToHashList(const word);

  //B-search equivalent but on a hash-list
  hashElem *hSearch(const hashList *, const word);

  void freeHashList(hashList *);

  hashList *hMergeSort(hashList *hL);
  hashList *hMerge(const hashList *, const hashList *);
#endif
