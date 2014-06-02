// Author: Emmanuel Odeke <odeke@ualberta.ca>
#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "radTrie.h"
#include "errors.h"
#include "radLoadWords.h"
#include "wordTransition.h"

#define tolower(x) (x | ('a' - 'A'))

#define BUF_SIZ 10 // Starting LinearizedTrie size for buffers
#define ALIGN_PAGE_SIZE(x, pageSize) (((x) + (pageSize) -1)/(pageSize) * (pageSize))

RTrie *fileToRTrie(const char *filePath) {
    RTrie *rt = NULL;

    int fd = open(filePath, 0, O_RDONLY);
    if (fd < 0) {
        raiseError(strerror(errno));
    }
    else {
        struct stat st;
        if (fstat(fd, &st) != 0) {
            close(fd);
            raiseError(strerror(errno));
        }
        else {
            int pageSize = sysconf(_SC_PAGE_SIZE);
            int mapLength = ALIGN_PAGE_SIZE(st.st_size, pageSize);

        #ifdef DEBUG
            printf("pageSize: %d mapLength: %d stSize: %d\n", pageSize, mapLength, st.st_size);
        #endif

            char *buf = mmap(NULL, mapLength, PROT_READ, MAP_SHARED, fd, 0);
            if (buf == MAP_FAILED) {
                raiseError(strerror(errno));
            }
            else {
                register int i=0, j;
                char c;
                while (i < st.st_size) {
                    int wBufLen = 10;
                    j = 0;
                    char *wordIn = (char *)malloc(sizeof(char) * wBufLen);
                    while (isalpha(c = buf[i++])) {
                        if (j >= wBufLen) {
                            wBufLen += 10;
                            wordIn = (char *)realloc(wordIn, sizeof(char) * wBufLen);
                        }

                        wordIn[j++] = tolower(c);
                    }

                    if (! j) {
                        if (wordIn != NULL)
                            free(wordIn);
                    }
                    else {
                        wordIn = (char *)realloc(wordIn, sizeof(char) * (j + 1));
                        wordIn[j] = '\0';
                        unsigned long int h = pjwCharHash(wordIn);

                        void *gotten = get(rt, h);
                        if (gotten == NULL)
                            rt = put(rt, h, (void *)wordIn, True);
                        else
                            free(wordIn);
                    }
                }

                // Now for the clean-up
                if (munmap(buf, mapLength)) {
                    raiseWarning(strerror(errno));
                }
            }
        }

        close(fd);
    }

    return rt;
}

LinearizedTrie *linearizeRTrie(RTrie *rt, LinearizedTrie *mp) {
    if (rt != NULL && rt->keys != NULL) {
        if (rt->value != NULL)
            mp = addToHead(mp, rt->value);
        
        register unsigned int i=0, bitPos;
        while (i < BASE) {
            bitPos = 1<<i;
            if (rt->availBlock & bitPos) {
                mp = linearizeRTrie(*(rt->keys + i), mp);
            }
            ++i;
        }
    }

    return mp;
}

LinearizedTrie *destroyLinearizedTrie(LinearizedTrie *lt) {
    // Won't be freeing the saved data since it's rightful owner is the Trie that got linearized
    LinearizedTrie *next = NULL;
    while (lt != NULL) {
        next = lt->next;
        free(lt);
        lt = next;
    }

    return lt;
}

Element *matches(
  const char *query, RTrie *dict, 
  const unsigned int ownRank, const double percentMatch
) {
    Element *matchL = NULL;
    if (query != NULL && dict != NULL) {
        double threshHold = ownRank * percentMatch;
        LinearizedTrie *lt = NULL, *trav;
        lt = linearizeRTrie(dict, lt);

        trav = lt;
        while (trav != NULL) {
            if (trav->value != NULL) {
                int rank = getRank(query, (char *)trav->value);
                if (rank >= threshHold) {
                    matchL =\
                        addToHeadWithRank(matchL, trav->value, (double)rank/ownRank);
                }
            }

            trav = trav->next;
        }

        lt = destroyLinearizedTrie(lt);
    }

    return matchL;
}

Element *getCloseMatches(const char *query, RTrie *dict, const double percentMatch) {
    if (query == NULL || dict == NULL) {
        return NULL;
    } else {
        // First check if the query exists in the dict
        void *check = get(dict, pjwCharHash(query));

        // In case of any collisions, strcmp should help sort things out
        if (check != NULL && strcmp(query, (char *)check) == 0) {
            Element *matchList = NULL;
            matchList = addToHead(matchList, check);
            return matchList;
        }
        else {
            int ownRank = getRank(query, query);
            return matches(query, dict, ownRank, percentMatch);
        }
    }
}

void printLinearizedTrie(LinearizedTrie *lt) {
    if (lt != NULL) {
        LinearizedTrie *trav = lt;
        while (trav != NULL) {
            printf("%s\n", (char *)trav->value);
            trav = trav->next;
        }
    }
}

#ifdef TEST_LOAD_WORDS
int main() {
    RTrie *rt = fileToRTrie("../../resources/enable1.txt");
    printf("\033[47mDone loading words\033[00m\n");

    LinearizedTrie *linear = NULL;
    linear = linearizeRTrie(rt, linear);
    // printLinearizedTrie(linear);
    linear = destroyLinearizedTrie(linear);

    Element *matches = getCloseMatches("matchong\0", rt, 0.6);
    printLinearizedTrie(matches);
    matches = destroyLinearizedTrie(matches);

    rt = destroyRTrie(rt);
    return 0;
}
#endif
