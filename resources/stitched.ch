// Author: Emmanuel Odeke <odeke@ualberta.ca>
#ifndef _ERRORS_H
#define _ERRORS_H
  #include <stdio.h>

  typedef enum {
    TypeError, ValueError, IndexError, SyntaxError, BufferOverFlow,
    AssertionError, NullPointerException, IndexOutOfBoundsException,
    ZeroDivisionException, CorruptedDataException
  } Exception;
  
  #define raiseWarning(errMsg){\
    fprintf(stderr,"\033[31m%s on line %d in function '%s' file '%s'\033[00m\n",\
      errMsg,__LINE__,__func__,__FILE__);\
  }

  #define throwException(errCode,errMsg){\
    if (errMsg != NULL){\
      fprintf(stderr, "%s\n", #errMsg);\
    }\
    raiseWarning(#errCode);\
    exit(-1);\
  }
 
  #define assert(validExpression){\
    if (! validExpression)\
      raiseError(validExpression);\
  }

  #define raiseError(args) {\
    fprintf(stderr, "Traceback most recent call at line: %d ", __LINE__);\
    fprintf(stderr, "of file: %s\n\033[31m%s\033[00m\n", \
         __FILE__, #args);\
    exit(-2);\
  }

  #define raiseExceptionIfNull(expression){\
    if (! expression)\
      throwException(NullPointerException, expression);\
  }
#endif
// Author: Emmanuel Odeke <odeke@ualberta.ca>
#ifndef _HASHLIST_H
#define _HASHLIST_H

  #define INIT_HASH_LIST_SIZE 100000
  typedef int hashValue;

  typedef enum {
    False=0, True=1
  } Bool;

  typedef struct Element_ {
    void *value;
    struct Element_ *next;
  } Element;

  typedef struct {
    int capacity, size;
    Element **list;
  } HashList;

  // Returns True if the element's next entry is non-NULL
  inline Bool hasNext(Element *);

  // Next attribute accessor
  inline Element *getNext(Element *);
  inline int getSize(HashList *hl);

  Element *initElement(Element *);
  HashList *initHashListWithSize(HashList *hl, const int size);
  HashList *initHashList(HashList *);

  Element *addToHead(Element *sl, void *data);
  Element *addToTail(Element *sl, void *data, const Bool overWriteOnDup);

  Element **get(HashList *hl, hashValue hashCode);
  Element *pop(HashList *hM, const hashValue hashCode);

  // Returns the number of values freed
  long int destroySList(Element *sl);

  // Returns the number of values freed
  long int destroyHashList(HashList *hl);

  // Miscellaneous
  hashValue pjwCharHash(const char *srcW);
#endif
// Author: Emmanuel Odeke <odeke@ualberta.ca>
#ifndef _HLRU_H
#define _HLRU_H
  #include "hashList.h"

  #define Cache LRU
  typedef HashList LRU;
#endif
// Author: Emmanuel Odeke <odeke@ualberta.ca>
#ifndef _MUTEX_COND_PAIR_H
#define _MUTEX_COND_PAIR_H
  #include <pthread.h>
  typedef struct {
    pthread_cond_t *condVar;
    pthread_mutex_t *mutex;
  } MutexCondPair;

  MutexCondPair *allocMutexCondPair(void);
  MutexCondPair *freeMutexCond(MutexCondPair *mcPair);
  MutexCondPair *initMutexCondPair(MutexCondPair *mcPair);
#endif
// Author: Emmanuel Odeke <odeke@ualberta.ca>
#ifndef _SCROOGE_H
#define _SCROOGE_H
  #include "HLRU.h"
  #include "MutexCondPair.h"

  typedef struct {
    LRU *consumerList;
    unsigned int maxCapacity;
    unsigned int consumerCount;
    MutexCondPair **mutexCondList; 
    void (*workGenerate)(void *);
  } Producer;

  Producer *allocProducer();
  Producer *initProducer(Producer *prod, const unsigned int capacity);

  int insertJob(Producer *prod, void *job, const int jobId);
#endif
// Author: Emmanuel Odeke <odeke@ualberta.ca>
// HashList implementation

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "errors.h"
#include "hashList.h"

#ifdef DEBUG
  #undef DEBUG
#endif

#define HANDLE_COLLISIONS
#define EXHIBIT_COLLISION
#define EXHIBIT_GET_BY_REFERENCE

inline Bool hasNext(Element *e) { return e != NULL && e->next != NULL; }
inline Element *getNext(Element *e) { return e == NULL ? NULL : e->next; }
inline int getSize(HashList *hl) { return hl == NULL ? 0 : hl->size; }

Element *addToTail(Element *sl, void *data, const Bool overWriteOnDup) {
  if (sl == NULL) {
    sl = initElement(sl);
    sl->value = data;
  } else if (sl->value != data) {
    sl->next = addToTail(sl->next, data, overWriteOnDup);
  } else {
    if (overWriteOnDup) {
      sl->value = data;
    } else {
      //Do something interesting eg store number of visits
    }
  }

  return sl;
}

Element *addToHead(Element *sl, void *data) {
  if (sl != NULL) {
    sl = initElement(sl);
  }

  Element *newElem = NULL;
  newElem = initElement(newElem);
  newElem->value = data;
  newElem->next = sl;
  sl = newElem;

  return sl;
}

Element *initElement(Element *elem) {
  if (elem == NULL) {
    elem = (Element *)malloc(sizeof(Element));
  } 

  elem->next = NULL;
  elem->value = NULL;

  return elem;
}

HashList *initHashListWithSize(HashList *hl, const int size) {
  if (hl == NULL) {
    hl = (HashList *)malloc(sizeof(HashList));
    raiseExceptionIfNull(hl);
  }

  if (size > 0) {
    hl->size = size;
  #ifdef DEBUG
    printf("%s:: %d\n", __func__, hl->size);
  #endif
    if ((hl->list = (Element **)malloc(sizeof(Element *) * hl->size)) == NULL) {
      free(hl);
      raiseError(
      "Run out of memory, trying to create space for a hashlist's list attribute"     );
    }

    // All elements set to NULL
    Element **listIter = hl->list, **end = hl->list + hl->size;
    while (listIter != end) {
    #ifdef DEBUG
      printf("lIt: %p end: %p\n", listIter, end);
    #endif
      *listIter++ = NULL;
    }
  }

  return hl;
}

HashList *initHashList(HashList *hl) {
  return initHashListWithSize(hl, INIT_HASH_LIST_SIZE);
}

void insertElem(HashList *hl, void *data, const hashValue hashCode) {
  if (hl == NULL) {
    raiseError("Cannot add elements to a NULL hashList");
  } 

  if (hl->size == 0) {
    printf("HashList size is zero, initializing it now\n");
    hl = initHashList(hl);
    assert(hl->size != 0);
  }

  int elemIndex = hashCode % hl->size;
#ifdef DEBUG
  printf("HashCode: %d Element at Index: %p\n", hashCode, hl->list[elemIndex]);
#endif
  if (hl->list[elemIndex] == NULL) { // We've found the first entry matching that hash
    hl->list[elemIndex] = initElement(hl->list[elemIndex]);
    hl->list[elemIndex]->value = data;
  } else {
    #ifdef HANDLE_COLLISIONS
      // Always update to the latest value
      hl->list[elemIndex] = addToTail(hl->list[elemIndex], data, True);
    #else 
      hl->list[elemIndex]->value = data;
    #endif
  }
}

Element **get(HashList *hl, hashValue hashCode) {
  return (hl == NULL || ! hl->size || hl->list == NULL) \
	  ? NULL : &(hl->list[hashCode % hl->size]);
}

long int destroySList(Element *sl) {
  long int nValueFrees = 0;
  if (sl != NULL) { 
    // printf("Sl == NULL: %d\n", sl != NULL);
    Element *tmp;
    while (sl != NULL) { 
      tmp = sl->next;
    #ifdef DEBUG
      printf("Freeing:: curHead: %p Next: %p\n", sl, tmp);
    #endif
      if (sl->value != NULL) {
	free(sl->value);
	++nValueFrees;
      }

      free(sl);

      sl = tmp;
    }
    sl = NULL;
  }

  return nValueFrees;
}

Element *pop(HashList *hM, const hashValue hashCode) {
  Element *pElement = NULL;

  if (getSize(hM)) {
    unsigned int calcIndex = hashCode % getSize(hM);
    pElement = hM->list[calcIndex];
    hM->list[calcIndex] = NULL;
  }

#ifdef DEBUG
  printf("hPElement: %p\n", pElement);
#endif

  return pElement;
}

long int destroyHashList(HashList *hl) {
  long int nValueFrees = 0;
  if (hl != NULL) {
    int i;
    if (hl->list != NULL) {
      for (i=0; i < hl->size; ++i) {
	nValueFrees += destroySList(hl->list[i]);
      }

      free(hl->list);
      hl->list = NULL;
    }

    free(hl);
    hl = NULL;
  }

  return nValueFrees;
}

hashValue pjwCharHash(const char *srcW) {
  // PJW hashing algorithm
  hashValue h = 0;

  unsigned int g, i, 
	       srcLen = strlen(srcW) / sizeof(char);

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

#ifdef SAMPLE_RUN
int main() {
  HashList *hl = NULL;

  unsigned int hSize = 10000000;
#ifdef EXHIBIT_COLLISION
  hl = initHashListWithSize(hl, hSize/10);
#else
  hl = initHashListWithSize(hl, hSize);
#endif
  char *tmp = (char *)malloc(4);
  insertElem(hl, tmp, 2);

  int i;
  for (i=0; i < hSize; i++) {
    int *x = (int *)malloc(sizeof(int));
    *x = i;
    insertElem(hl, x, i);
  }

  printf("hl %p\n", hl);

#ifdef EXHIBIT_GET_REFERENCE
  Element **found = get(hl, 101);

  while (*found != NULL) {
    printf("FND: %d\n", *((int *)(*found)->value));

    Element *curHolder = *found;
    *found = (*found)->next;
     
    if (curHolder->value != NULL) free(curHolder->value); 
    free(curHolder);

  }
  if (found != NULL) free(*found);
#endif

  hashValue hTest = 101;
  Element *popd = pop(hl, hTest);

  while (popd != NULL) {
    Element *cur = popd;
    popd = getNext(popd);

    if (cur->value != NULL) {
      printf("Elem with hash: %d :: %d\n", hTest, *((int *)cur->value));
      free(cur->value);
    }

    free(cur);
  }

  destroyHashList(hl);
  return 0;
}
#endif
// Author: Emmanuel Odeke <odeke@ualberta.ca>
#include <stdio.h>
#include "HLRU.h"

#define DEFAULT_CACHE_SIZE 200

void destroyHLRU(LRU *cache) {
  destroyHashList(cache);
}

#ifdef SAMPLE_HLRU
int main() {
  LRU *cache = NULL;
  cache = initHashListWithSize(cache, DEFAULT_CACHE_SIZE);
  destroyHashList(cache);
  return 0;
}
#endif
// Author: Emmanuel Odeke <odeke@ualberta.ca>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "MutexCondPair.h"

#define execIfNotNull(var, func) {\
  if (var != NULL) func(var);\
}

MutexCondPair *allocMutexCondPair(void) {
  return (MutexCondPair *)malloc(sizeof(MutexCondPair));
}

MutexCondPair *initMutexCondPair(MutexCondPair *mcPair) {
  if (mcPair == NULL) {
    mcPair = allocMutexCondPair();
  }

  mcPair->mutex = NULL;
  mcPair->condVar = NULL;

  return mcPair;
}

MutexCondPair *freeMutexCond(MutexCondPair *mcPair) {
  if (mcPair != NULL) {
    execIfNotNull(mcPair->mutex, pthread_mutex_destroy);
    execIfNotNull(mcPair->condVar, pthread_cond_destroy);
    free(mcPair);
  }

  return mcPair;
}
#ifdef SAMPLE_MUTEX_COND_PAIR
int main() {
  return 0;
}
#endif
// Author: Emmanuel Odeke <odeke@ualberta.ca>
#include <stdio.h>
#include <stdlib.h>

#include "errors.h"
#include "Scrooge.h"
#include "MutexCondPair.h"

#define PRODUCER_CAPACITY 40
#define DEFAULT_PRODUCER_CAPACITY 10

#define DEBUG

Producer *allocProducer() {
  return (Producer *)malloc(sizeof(Producer));
}

Producer *initProducer(Producer *prod, const unsigned int capacity) {
  if (prod == NULL) {
    prod = allocProducer();
  }

  if (prod == NULL) raiseError("Run out of memory");

  prod->consumerCount = 0;
  prod->workGenerate = NULL;
  prod->maxCapacity = capacity;

  prod->consumerList = NULL;
  prod->consumerList = initHashListWithSize(prod->consumerList, capacity);
  prod->mutexCondList = NULL; 
  if (capacity) {
    prod->mutexCondList = \
      (MutexCondPair **)malloc(sizeof(MutexCondPair *) * capacity);
  } else {
  }

  return prod;
}

Producer *destroyProducer(Producer *prod) {
  if (prod != NULL) {
    if (prod->consumerList != NULL) {
      destroyHLRU(prod->consumerList);
      prod->consumerList = NULL;
    }

    if (prod->mutexCondList != NULL) {
      int mIndex;
      for (mIndex = 0; mIndex < prod->maxCapacity; ++mIndex) {
	prod->mutexCondList[mIndex] = freeMutexCond(prod->mutexCondList[mIndex]);
      }

      free(prod->mutexCondList);
    }

    free(prod);
    prod = NULL;
  }

  return prod;
}

void *generateData(void) {
  return NULL;
}

int insertJob(Producer *prod, void *job, const int jobId) {
#ifdef DEBUG
  printf("\033[33mIn %s\033[00m\n", __func__);
#endif
  insertElem(prod->consumerList, job, jobId);

  return 0;
}

googre

int main() {
  Producer *prod = NULL;
  prod = initProducer(prod, PRODUCER_CAPACITY);
#ifdef DEBUG
  printf("Scrooge!!\n");
#endif

  int i;
  for (i=0; i < 100; ++i) {
    int *intPtr = (int *)malloc(sizeof(int));
  #ifdef DEBUG
    printf("\033[94mInserting jobId: %d\n", i);
  #endif
    insertJob(prod, intPtr, i);
  }

  prod = destroyProducer(prod); 
  return 0;
}
