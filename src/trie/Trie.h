// Author: Emmanuel Odeke <odeke@ualberta.ca>
#ifndef _TRIE_H
#define _TRIE_H
  #define radixStart 'a'
  #define radixEnd   'z'
  #define radixSize (radixEnd - radixStart) 

  typedef struct Trie_ {
    int radixSz;
    unsigned int index;
    struct Trie_ **keys;
  } Trie;

  // Get a heap's worth of a Trie
  Trie *allocTrie();

  Trie *destroyTrie(Trie *tr);

  Trie *createTrie(const int index);
 
  // Utilities here 

  int resolveIndex(const char c);

  inline int bitLen(unsigned int n);
#endif
