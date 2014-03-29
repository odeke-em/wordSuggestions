// Author: Emmanuel Odeke <odeke@ualberta.ca>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "pTrie.h"

void *pTrieOp(void *data) {
    if (data != NULL) {
        struct TrieChunk *tChunk = (struct TrieChunk *)data;
        while (tChunk->start < tChunk->end && tChunk->start < tChunk->limit) {
        #ifdef DEBUG
            printf("start: %p end: %p limit: %p\n", tChunk->start, tChunk->end, tChunk->limit); 
        #endif
            printf("\033[47mId: %d\033[00m\n", tChunk->id);

            // Proof Of Concept: so not caring about race conditions for output file descriptor
            exploreTrie(*(tChunk->start), "\0", stdout); 
            ++tChunk->start;
        }
    }

    return NULL;
}

void thTrieExplore(Trie *t, unsigned int thCount) {
    if (t != NULL && t->keys != NULL && thCount > 1) {
        unsigned int i=0, chunkSize = radixSize / thCount,
                     residual = radixSize - (chunkSize * thCount),
                     maxIndex = thCount + (residual > 0), chunkIndex=0;
        
        pthread_t thList[maxIndex];
        struct TrieChunk chunkList[maxIndex];
        Trie **tList[maxIndex], **limit = t->keys + radixSize;

        printf("maxIndex: %d chunkSize: %d\n", maxIndex, chunkSize);
        while (i < maxIndex && chunkIndex < radixSize) {
        #ifdef DEBUG
            printf("i %d chunkIndex: %d\n", i, chunkSize);
        #endif
            chunkList[i].id = i;
            chunkList[i].limit = limit;
            chunkList[i].start = t->keys + chunkIndex;
            chunkIndex += chunkSize;
            chunkList[i].end = t->keys + chunkIndex;

            pthread_create(&thList[i], NULL, pTrieOp, &chunkList[i]);
            ++i;
        }

        unsigned int t;
        for (t=0; t < maxIndex && t < i; ++t) { // Start with the oldest added thread
            printf("\033[%s thread: %d\033[00m\n", pthread_join(thList[t], NULL) ? "31mFailed to join" : "32mSuccess joining", t);
        }
    }
}

int main() {
    FILE *ifp = fopen("../../resources/allWords.txt\0", "r");
    Trie *t = trieFromFile(ifp);

#ifdef PARALLEL_EXPLORE
    thTrieExplore(t, 13);
#else
    exploreTrie(t, "\0", stdout);
#endif

    t = destroyTrie(t);
    return 0;
}
