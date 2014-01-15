#ifndef _LRU_H
#define _LRU_H
  #include "list.h"

  #define Cache LRU // Achieve some type name aliasing
  typedef List Cache;

  // Save any elements that will be purged from the main cache
  // into purgedSav
  Cache *purgeAndSave(Cache *c, Cache **purgedSav);

  // Merely invokes purgeAndSave but with a NULL argument for purgedSav 
  Cache *purgeLRU(Cache *c);
 
  Cache *setTagValue(Cache *c, unsigned int tagValue);

  void *lookUpEntry(Cache *c, void *key, Comparator comp);
#endif
