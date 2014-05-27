// Author: Emmanuel Odeke <odeke@ualberta.ca>
#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "hashList.h"
#include "loadWords.h"
#include "wordTransition.h"

#define tolower(x) (x | ('a' - 'A'))

#define BUF_SIZ 10 // Starting element size for buffers
#define ALIGN_PAGE_SIZE(x, pageSize) (((x) + (pageSize) -1)/(pageSize) * (pageSize))

#define DEBUG_MATCH

char *getWord(FILE *ifp, int *lenStorage) {
  int bufLen = BUF_SIZ;
  char *wordIn = (char *)malloc(sizeof(char) * bufLen);
  assert(wordIn);

  char c;
  int idx = 0;
  while ((c = getc(ifp)) != EOF) {
    if ((idx + 1) >= bufLen) {
      bufLen += BUF_SIZ;
      wordIn = (char *)realloc(wordIn, sizeof(char) * bufLen);
    }

    if (isalpha(c)) { 
      wordIn[idx++] = tolower(c);
    } else break;
  }

  if (idx) {
    wordIn[idx++] = '\0';
    wordIn = (char *)realloc(wordIn, sizeof(char) * idx);

    if (lenStorage) *lenStorage = idx;

    return wordIn;
  } else {
    free(wordIn);
    return NULL;
  }
}

HashList *loadWordsInFile(const char *filePath) {
  HashList *hl = NULL;
  long long int dictSize = 0;

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
        dictSize = st.st_size / AVERAGE_WORD_LEN;
        printf(" Dict size: %lld\033[00m\n", dictSize);
        hl = initHashListWithSize(hl, dictSize);

        if (hl == NULL) {
          raiseWarning("No memory available to create dict");
          exit(-1);
        }

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
            insertElem(hl, wordIn, pjwCharHash(wordIn));
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

  return hl;
}

void *subUnitMatch(void *data) {
  Element *resultL = NULL;
  MatchUnit *mu = (MatchUnit *)data;
  if (mu != NULL) {
    Element **trav = mu->start;
    while (trav < mu->end && trav < mu->absEnd) {
      if (*trav != NULL && (*trav)->value != NULL) {
        int rank = getRank(mu->query, (*trav)->value);
        if (rank >= mu->threshold) {
           resultL = addToHeadWithRank(resultL, (*trav)->value, (double)rank/mu->ownRank);
        }
      } 

      ++trav;
    }
  }

  return (void *)resultL;
}

Element *mergeEL(Element *from, Element *to) {
  if (from != NULL) {
    Element *saveNext = NULL;
    while (from != NULL) {
      to = addToHeadWithRank(to, from->value, from->rank);
      saveNext = from->next;
      free(from); // The data in from is inherited by 'to'
      from = saveNext;
    }
  }

  return to;
}

Element *matchesOnUniThreadOnMultiThread(
  const char *query, HashList *dict, const unsigned int ownRank,
  const double percentMatch, const unsigned int thCount
) {
  Element *mergedL = NULL;
  if (query != NULL && dict != NULL && dict->size >= 1 && thCount >= 1) {
    pthread_t thL[thCount];
    MatchUnit muL[thCount];

    unsigned int i=0, stepSize= 1 + dict->size/thCount, stepCounter=0;

    int threshold = ownRank * percentMatch;
    while (stepCounter < dict->size && i < thCount) {
      MatchUnit *mu = &muL[i];
      mu->query = (char *)query;
      mu->absEnd = dict->list + dict->size;
      mu->start =  dict->list + stepCounter;
      mu->ownRank = ownRank;
      mu->threshold = threshold; 

      stepCounter += stepSize;
      mu->end    =  dict->list + stepCounter;

      pthread_create(&thL[i++], NULL, subUnitMatch, mu);
    }

    unsigned int j;
    int joinStatus;
    for (j=0; j < i; ++j) {
      Element *dataRecv=NULL;
      joinStatus = pthread_join(thL[j], (void *)&dataRecv);
      if (joinStatus) {
        fprintf(stderr, "\033[91mFailed to join thread: %d. Status:: %d\n", j, joinStatus);
      }
      else {
        mergedL = mergeEL(dataRecv, mergedL);
      }
    }
  }

  return mergedL;
}
// Do not free data returned from this function as it just contains pointers
// to data associated with the dictionary that will be explicitly freed
Element *matchesOnUniThread(
  const char *query, HashList *dict, 
  const unsigned int ownRank, const double percentMatch
) {
  Element *matchL = NULL;
  if (query != NULL && dict != NULL) {
    double threshHold = ownRank * percentMatch;
    Element **trav = dict->list, **end = trav + getSize(dict);
    while (trav != end) {
      if (*trav != NULL && (*trav)->value != NULL) {
        int rank = getRank(query, (*trav)->value);
        if (rank >= threshHold) {
          matchL = \
            addToHeadWithRank(matchL, (*trav)->value, (double)rank/ownRank);
          } 
      }

      ++trav;
    }
  }

  return matchL;
}

Element *getCloseMatches(
  const char *query, HashList *dict, const double percentMatch
) {
  if (query == NULL || dict == NULL) {
    return NULL;
  } else {
    // First check if the query exists in the dict
    Element **matchList = get(dict, pjwCharHash(query)); 

    if (*matchList != NULL) { // An entry with the same hash value found
      // In case of any collisions, strcmp should help sort things out
      if ((*matchList)->value && strcmp(query, (*matchList)->value) == 0) {
        // Absolute match found
        return *matchList;
      }
    }

    int ownRank = getRank(query, query);

    long int processorCount = sysconf(_SC_NPROCESSORS_CONF);
    // printf("PC: %ld\n", processorCount);
    if (processorCount < 2) {
      return matchesOnUniThread(query, dict, ownRank, percentMatch);
    }
    else {
      return matchesOnUniThreadOnMultiThread(
        query, dict, ownRank, percentMatch, processorCount
      );
    }
  }
}

#ifdef SAMPLE_RUN
int main() {
  HashList *hl = loadWordsInFile("../../resources/wordlist.txt");
  HashList *curFileHL = loadWordsInFile(__FILE__);

  int i;
  printf("hl: %p\n", hl);
  printf("curFileHl: %p\n", curFileHL);
  for (i = 0; i < getSize(curFileHL); ++i) {
    if (curFileHL->list[i] != NULL) {
      char *value = (char *)(curFileHL->list[i]->value);
      Element *match = getCloseMatches(value, hl, 0.6);
      printf("For: %s %p\n", value, curFileHL->list[i]);
      while (match != NULL) {
         printf("\t%s\n", (char *)match->value);
         match = getNext(match);
      }
    }
  }

  destroyHashList(hl);
  destroyHashList(curFileHL);
  return 0;
}
#endif
