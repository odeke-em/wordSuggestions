/*Author: Emmanuel Odeke <odeke@ualberta.ca>*/

#include <stdio.h>
#include <pthread.h>
#include "wordSearch.h"

#define EXIT_CHAR '-'
#define BUF_SIZ 30
#define MAX_PATH 60

#define LEARNT_WORDS_PATH "learnt_words.txt"

void  spellCheck(FILE *, char *); 

static pthread_cond_t cond_t = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t main_tx = PTHREAD_MUTEX_INITIALIZER;

typedef struct{
  void (*func)(FILE*, char *);
  char *queryWord;
  FILE *dictFP;
} funcStruct;

void *runFunc(void *data ){
  funcStruct *f = (funcStruct *)data;
  pthread_mutex_lock(&main_tx);
  f->func(f->dictFP, f->queryWord);
  pthread_cond_signal(&cond_t);

  pthread_mutex_unlock(&main_tx);
  return NULL;
}
struct procStruct{
  Bool *processDone;
};

void *timeScreen(void *data){
  struct procStruct *p = (struct procStruct *)data;
  int i=0;
  int nDots=0, MAX_DOTS=4;
  while (*(p->processDone) == False ){
    if (nDots >= MAX_DOTS){
       fprintf(stderr, "%c[2K",27);
       nDots = 0;
    }

    fprintf(stderr,"\nProcessing");
    
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

int main(){
  Bool doneReading = False;
  pthread_t timer_t;
  pthread_t main_th;

  char *s = "wordlist.txt";
  FILE *dictFP = fopen(s, "r");
  char query[MAX_PATH];
  Bool *procDone = (Bool *)malloc(sizeof(Bool));

  funcStruct p;
  p.func= spellCheck;
  p.dictFP = dictFP;

  struct procStruct procSt;
  procSt.processDone = procDone; 
  while (! doneReading){
    fprintf(stderr,"\n");
    fprintf(stderr, 
      "%c as the first character exists the program\n",EXIT_CHAR);
    fprintf(stderr,"\nQuery "); 

    if (getLine(query, MAX_PATH) == EOF ){
      fprintf(stderr,"EOF encountered. Done reading\n");
      exit(0);
    };

    if (*query == EXIT_CHAR)
      break;

    *procDone = False;
    p.queryWord = query;

    pthread_create(&main_th, NULL, runFunc, &p);
    pthread_create(&timer_t, NULL, timeScreen, &procSt);
    pthread_cond_wait(&cond_t, &main_tx);

    *procDone = True;
    sleep(1);
    fprintf(stderr,"Done.\n");
  }

  free(procDone);
  pthread_mutex_destroy(&main_tx);
  pthread_cond_destroy(&cond_t);

  fclose(dictFP);
  return 0;
}

void spellCheck(FILE *dictFP, char *srcWord){
  #ifdef DEBUG
    fprintf(stderr,"Query %s func %s\n",srcWord,__func__);
    fflush(stderr);
  #endif

  Node *storage = NULL;
  #ifdef DEBUG
    fprintf(stderr,"srcWord %s\n",srcWord);
  #endif

  //Definition of function 'loadWord(...)'
  //Node *loadWord(FILE *,Node *,char *query,Bool LEN_MATCH_BOOL, 
  //                             Bool FIRST_LETTER_MATCH);

  //Add to 'storage' those words that have a ranked similarity to the word 
  //under scrutiny
  storage  = loadWord(dictFP, stdout, storage, srcWord, False, False);
  //nodePrint(storage);

  //And give unto OS, what belongs to OS -- release memory
  nodeFree(storage);
}
