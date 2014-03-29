#ifndef _P_TRIE_H
#define _P_TRIE_H
    // Author: Emmanuel Odeke <odeke@ualberta.ca>

    #include "Trie.h"
    struct TrieChunk{
        Trie **start, **end, **limit;
        int id;
    };

    void thTrieExplore(Trie *t, unsigned int thCount);
#endif
