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
    unsigned int h = 0, g, i, srcLen = strlen(srcW) / sizeof(char);

    for (i=0; i < srcLen; ++i) {
        h = (h << 4) + srcW[i];
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
    rt->availBlock = 0;
    return rt;
}

RTrie *put(RTrie *rt, unsigned long int hash, void *data, Bool heapBool) {
    if (rt == NULL) {
        rt = newRTrie();
    }

    if (rt->keys == NULL) {
        rt->keys = (RTrie **)malloc(sizeof(RTrie *) * BASE);
    }

    unsigned int residue = hash % BASE;
    unsigned int bitPos = 1 << residue;
    hash /= BASE;

    if (! (rt->availBlock & bitPos)) {
        rt->availBlock |= bitPos;
        // printf("bP: %d residue: %d hash: %ld avB: %d\n", bitPos, residue, hash, rt->availBlock);
        *(rt->keys + residue) = NULL;
    }

    if (hash || residue) {
        *(rt->keys + residue) = put(*(rt->keys + residue), hash, data, heapBool);
    }
    else {
        // TODO: Define what to do if the value was already present
        // printf("setting eos for %p\n", data);
        // printf("origValue: %s newValue: %s\n", (char *)rt->value, (char *)data);
        rt->EOS = 1;
        rt->value = data;
        rt->valueIsHeapd = heapBool;
    }

    return rt;
}

void *__rAccess(RTrie *rt, unsigned long int hash, Bool isGetOp) {
    if (rt == NULL)
        return NULL;
    else {
        unsigned int residue = hash % BASE, bitPos;
        bitPos = 1 << residue;
        hash /= BASE;
        if (! (rt->availBlock & bitPos))
            return NULL;
        else {
            if (hash || residue)
                return __rAccess(*(rt->keys + residue), hash, isGetOp);
            else {
                // printf("EOS: %d data: %p\n", rt->EOS, rt->value);
                if (rt->EOS) {
                    if (isGetOp)
                        return rt->value;
                    else {
                        void *popd = rt->value;
                        rt->value = NULL;
                        rt->EOS = False;
                        return popd;
                    }
                }
                else
                    return NULL;
            }
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
            unsigned int i=0, bitPos;
            while (i < BASE) {
                bitPos = 1<<i;
                if (rt->availBlock & bitPos) {
                    // printf("bitPos: %d avB: %d\n", bitPos, rt->availBlock);
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

        // printf("Freeing rt: %p\n", rt);
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
