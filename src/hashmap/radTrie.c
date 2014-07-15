// Author: Emmanuel Odeke <odeke@ualberta.ca>
#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "radTrie.h"
#include "errors.h"

inline RTrie *allocRTrie(void) {
    return (RTrie *)malloc(sizeof(RTrie));
}

unsigned int pjwCharHash(const char *srcW) {
    // PJW hashing algorithm
    unsigned int h, g;

    for (h=0; *srcW; ++srcW) {
        h = (h << 4) + *srcW;
        g = h & 0xf0000000;
        if (g) {
            h ^= (g >> 24);
            h ^= g;
        }
    }

    return h;
}

RTrie *newRTrie(void) {
    RTrie *rt = allocRTrie();
    assert(rt);

    rt->EOS = 0;
    rt->keys = NULL;
    rt->value = NULL;
    rt->meta  = NULL;
    return rt;
}

RTrie *put(RTrie *rt, unsigned long int hash, void *data, Bool heapBool) {
    if (rt == NULL) {
        rt = newRTrie();
        rt->keys = (RTrie **)calloc(BASE, sizeof(RTrie *));
    }

    RTrie *rTrav = rt;

    register unsigned int residue;
    do {
        residue = hash % BASE;
        if (residue < 0 || residue >= BASE)
            goto doneHere;

        if (*(rTrav->keys + residue) == NULL)
            *(rTrav->keys + residue) = newRTrie();

        rTrav = *(rTrav->keys + residue);
        if (rTrav->keys == NULL)
            rTrav->keys = (RTrie **)calloc(BASE, sizeof(RTrie *));

        hash /= BASE;
    } while (hash);

    // TODO: Define what to do if the value was already present
    // printf("setting eos for %p\n", data);
    // printf("origValue: %s newValue: %s\n", (char *)rt->value, (char *)data);
    rTrav->EOS = 1;
    rTrav->value = data;
    rTrav->valueIsHeapd = heapBool;

    doneHere:
        return rt;
}

void *__rAccess(RTrie *rt, unsigned long int hash, Bool isGetOp) {
    RTrie *rTrav = rt;

    do {
        if (rTrav == NULL || rTrav->keys == NULL)
            return NULL;

        rTrav = *(rTrav->keys + (hash % BASE));
        hash /= BASE;
    } while (hash);
      
    if (! (rTrav && rTrav->EOS))
        return NULL;
    else {
        if (isGetOp)
            return rTrav->value;
        else {
            void *popd = rTrav->value;
            rTrav->value = NULL;
            rTrav->EOS = False;
            return popd;
       }
    }
}

void *get(RTrie *rt, unsigned long int hash) {
    return __rAccess(rt, hash, True);
}

void *pop(RTrie *rt, unsigned long int hash) {
    return __rAccess(rt, hash, False);
}

RTrie *destroyRTrie(RTrie *rt) {
    if (rt != NULL) {
        if (rt->keys != NULL) {
            unsigned int i=0;
            while (i < BASE) {
                if (*(rt->keys + i) != NULL) {
                    *(rt->keys + i) = destroyRTrie(*(rt->keys + i));
                }
                ++i;
            }

            free(rt->keys);
            rt->keys = NULL;
        }

        if (rt->valueIsHeapd && rt->value != NULL) {
            free(rt->value);
            rt->value = NULL;
        }

        // Note the 'meta' attribute will be freed by it's rightful user
        if (rt->meta != NULL)
            raiseWarning("Possible memory leak since 'meta' attribute was not freed");

        free(rt);
        rt = NULL;
    }

    return rt;
}


#ifdef TEST_RAD_TRIE
int main() {
    return 0;
}
#endif
