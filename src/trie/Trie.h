// Author: Emmanuel Odeke <odeke@ualberta.ca>
#ifndef _TRIE_H
#define _TRIE_H
  #define radixStart 'a'
  #define radixEnd   'z'
  #define radixSize (radixEnd - radixStart) 

  typedef enum TrieTag_ {
    StackD=0, HeapD=1
  } TrieTag;

  typedef struct Trie_ {
    int radixSz;
    void *payLoad;
    TrieTag loadTag;
    struct Trie_ **keys;
  } Trie;

  // Get a heap's worth of a Trie
  Trie *allocTrie();

  Trie *destroyTrie(Trie *tr);

  Trie *createTrie();
  
  // Keep following allowing and only deposit the load once at
  // the sequence's end ie once we've hit a '\0' 
  Trie *addSequenceWithLoad(
    Trie *tr, const char *seq, void *load, const TrieTag tag
  );
  // Normal routine adding except that the payload is set to NULL
  Trie *addSequence(Trie *tr, const char *seq);

  void *searchTrie(Trie *tr, const char *seq);

  void exploreTrie(Trie *t, const char *pAxiom);

  // Utilities here 
  Trie *trieFromFile(FILE *ifp);

  int resolveIndex(const char c);
#endif
