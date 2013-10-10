/*
  Author: Emmanuel Odeke <odeke@ualberta.ca>
*/
#include <pthread.h>
#include "../include/constants.h"
#include "../include/wordSearch.h"

void  spellCheck(const wordArrayStruct *, const word); 

static pthread_cond_t cond_t = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t main_tx = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
  void (*func)(const wordArrayStruct *, const word);
  word queryWord;
  wordArrayStruct *wordArraySt;
} funcStruct;

void *runFunc(void *data) {
  funcStruct *f = (funcStruct *)data;

  pthread_mutex_lock(&main_tx);

  f->func(f->wordArraySt, f->queryWord);

  pthread_cond_signal(&cond_t);
  pthread_mutex_unlock(&main_tx);

  return NULL;
}

struct procStruct {
  Bool *processDone;
};

void *timeScreen(void *data) {
  struct procStruct *p = (struct procStruct *)data;
  int i=0;
  int nDots=0, MAX_DOTS=4;
  while (*(p->processDone) == False ){
    if (nDots >= MAX_DOTS){
       fprintf(stderr, "%c[2K",27);
       nDots = 0;
    }

    fprintf(stderr,"Processing");
    
    int tempI;
    for (tempI=0; tempI<nDots; ++tempI)
      fprintf(stderr,".");

    fprintf(stderr, "\r");
    fflush(stderr);
    sleep(1);
    ++nDots;
    ++i;
  }
  fprintf(stderr,"\n%d seconds spent\n",i);
  return NULL;
}

int main() {
  Bool doneCorrecting = False;
  pthread_t timer_t;
  pthread_t main_th;

  char query[MAX_PATH];
  Bool *procComplete = (Bool *)malloc(sizeof(Bool));

  funcStruct callFuncSt;
  callFuncSt.func= spellCheck;

  wordArrayStruct *wASt = wordsInFile(DICTIONARY_PATH);
  callFuncSt.wordArraySt= wASt;

  struct procStruct procSt;
  procSt.processDone = procComplete; 

  while (! doneCorrecting){
    fprintf(stderr, 
     "\n\033[33m%c as the first character exists the program\033[00m\n\nQuery ",
     EXIT_CHAR
    );

    if (getLine(query, MAX_PATH) == EOF ){
      fprintf(stderr,"EOF encountered. Done reading\n");
      exit(0);
    };

    if (*query == EXIT_CHAR)
      break;

    *procComplete = False;
    callFuncSt.queryWord = query;

    pthread_create(&main_th, NULL, runFunc, &callFuncSt);
    pthread_create(&timer_t, NULL, timeScreen, &procSt);
    pthread_cond_wait(&cond_t, &main_tx);

    *procComplete = True;
    sleep(1);
    fprintf(stderr,"Done.\n");
  }

  free(procComplete);
  freeWordArrayStruct(wASt);

  pthread_mutex_destroy(&main_tx);
  pthread_cond_destroy(&cond_t);

  return 0;
}

void spellCheck(const wordArrayStruct *wASt, const word srcWord) { 
  #ifdef DEBUG
    fprintf(stderr,"Query %s func %s\n",srcWord,__func__);
    fflush(stderr);
  #endif

  Node *storage = NULL;
  #ifdef DEBUG
    fprintf(stderr,"srcWord %s\n",srcWord);
  #endif

  wordMatchCriteria matchCriteria;
  matchCriteria.lenMatch_bool = False; 
  matchCriteria.firstLetterMatch_bool = False; 

  storage = getSuggestions(wASt, stdout, storage, srcWord, matchCriteria);

  nodeFree(storage);
}
