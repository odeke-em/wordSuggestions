// Author: Emmanuel Odeke <odeke@ualberta.ca>
#ifndef _TRIE_H
#define _TRIE_H
  #include <stdio.h>
  #define alphaStart 'a'
  #define alphaEnd   'z'
  #define alphaDiff  alphaEnd - alphaStart
  #define space ' '
  #define radixSize (alphaDiff + 2) // Accomodate a space char' '
						// and the end itself

  typedef enum TrieTag_ {
    StackD=0, HeapD=1
  } TrieTag;

  typedef struct Trie_ {
    int radixSz;
    void *payLoad;
    TrieTag loadTag;
    unsigned int EOS:1; //End-Of-Sequence
    struct Trie_ **keys;
  } Trie;

  // Get a heap's worth of a Trie
  Trie *allocTrie();
  
  // Pass in the custom function to free the payload data
  Trie *destroyTrieAndPayLoads(Trie *t, void *(*loadFreer)(void *));

  // Pass the payload freer as NULL
  Trie *destroyTrie(Trie *tr);

  Trie *createTrie();
  
  // Keep following allowing and only deposit the load once at
  // the sequence's end ie once we've hit a '\0' 
  Trie *addSequenceWithLoad(
    Trie *tr, const char *seq, void *load, const TrieTag tag
  );
  // Normal routine adding except that the payload is set to NULL
  Trie *addSequence(Trie *tr, const char *seq);

  int searchTrie(const Trie *tr, const char *seq, void **ptrSav);

  void exploreTrie(const Trie *t, const char *pAxiom, FILE *ofp);

  // Utilities here 
  Trie *trieFromFile(FILE *ifp);

  int resolveIndex(const char c);
#endif
