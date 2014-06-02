// Author: Emmanuel Odeke <odeke@ualberta.ca>

// Hash map implementation in the form of a trie.
// Helps solve the limited size of hashlists as well enables lookup, adding and removal
// For every step takes the modulo remainder of the hash
// given in, creating a path, the purpose is to preserve memory ie setting pointers of keys
// only if ever __rAccessed otherwise, they are left uninitialized
// given h = WXYZ and base M. Step 1: h % M = Z => Create path in Z, h /= M
//                            Step 2: h % M = Y => Create path in Y, h /= M
//                      ...   Step 4: h % M = W => Set payload to data, and then EOS to True

#ifndef _RAD_TRIE_H
#define _RAD_TRIE_H
    #ifndef _BOOL_D
    #define _BOOL_D
    typedef enum {
        False=0, True=1
    } Bool;
    #endif // _BOOL_D

    static unsigned int BASE = 10;
    typedef struct RTrie_ {
        void *value;
        Bool EOS:1;             // EndOfSequence
        Bool valueIsHeapd:1;
        struct RTrie_ **keys;
        unsigned int availBlock; // Each set bit will indicate the presence of a key in that position
    } RTrie;

    inline RTrie *allocRTrie(void);

    RTrie *newRTrie(void);
    RTrie *destroyRTrie(RTrie *);

    void *get(RTrie *rt, unsigned long int hash);
    void *pop(RTrie *rt, unsigned long int hash);
    void *__rAccess(RTrie *rt, unsigned long int hash, Bool isGetOp);
    RTrie *put(RTrie *rt, unsigned long int hash, void *value, Bool heapBool);

    unsigned int pjwCharHash(const char *srcW);
#endif
