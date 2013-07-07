/*Author: Emmanuel Odeke <odeke@ualberta.ca>*/

#include <stdio.h>
#include <pthread.h>
#include "wordSearch.h"
#include <sys/signal.h>

#define EXIT_CHAR '-'
#define LEARNT_WORDS_PATH "learnt_words.txt"

static size_t MAX_PATH = 60;
static pthread_cond_t cond_t = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t main_tx = PTHREAD_MUTEX_INITIALIZER;

typedef struct{
  void (*func)(char *);
  char *path;
} funcStruct;


void *runFunc( void *data ){
  funcStruct *f = (funcStruct *)data;
  pthread_mutex_lock(&main_tx);
  f->func(f->path);
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
  while ( *(p->processDone) == False ){
    if (nDots >= MAX_DOTS){
       fprintf(stderr, "%c[2K",27);
       nDots = 0;
    }

    fprintf(stderr,"Processing");
    
    int tempI;
    for (tempI=0; tempI<nDots; ++tempI)
      fprintf(stderr,".");
    fprintf(stderr, "\r");
    fflush(stdout);
    sleep(1);
    ++nDots;
    ++i;
  }
  fprintf(stdout," %d second%c spent ",i, ( i != 1 ? 's' : ' '));
  return NULL;
}

void  autoCorrect(char *); 
char *getWord(FILE *fp);

int getLine(char *s, int max){
  int i=0;
  char c=EOF;
  while ((i < max) && ((c = getchar()) != EOF)){
      if ((c == ' ') || (c == '\n' && putchar(c))){
	s[i] = '\0';
	break;
      }
      s[i] = c;	
      i++;
  }
  return (( c == EOF ) ? EOF : i );
}

int main(){
  Bool doneReading = False, validfile;
  pthread_t timer_t;
  pthread_t main_th;

  char *s = "wordlist.txt";
  FILE *fp = fopen(s, "r");
  char *path=(char *)malloc(sizeof(char)*MAX_PATH);
  Bool *procDone = (Bool *)malloc(sizeof(Bool));

  funcStruct p;
  p.func= autoCorrect;

  struct procStruct procSt;
  procSt.processDone = procDone; 
  while (! doneReading){
    fprintf(stderr,"\n");
    fprintf(stderr, 
      "%c as the first character exists the program\n",EXIT_CHAR);
    fprintf(stderr,"Enter the file whose words will "); 
    fprintf(stderr, "be parsed and compared against those from a dictionary: ");

    if ((fscanf(stdin,"%s",path) != 1) || (*path == EOF)){ 
      fprintf(stderr,"EOF encountered. Done reading\n");
      exit(0);
    };

    if (*path == EXIT_CHAR)
      break;

    validfile = isValidFile(path);
    if(validfile){
      *procDone = False;
      p.path = path;
      pthread_create(&main_th, NULL, runFunc, &p);
      pthread_create(&timer_t, NULL, timeScreen, &procSt);
      pthread_cond_wait(&cond_t, &main_tx);

      //pthread_kill(timer_t,SIGINT);
      //pthread_kill(main_th,SIGINT);

      *procDone = True;
      sleep(1);
      fprintf(stderr,"Done.\n");
    }else
      fprintf(stderr,"Non-existant path %s\n",path);
  }

  free(procDone);
  pthread_mutex_destroy(&main_tx);
  pthread_cond_destroy(&cond_t);

  fclose(fp);
  return 0;
}

void autoCorrect(char *srcTextPath){
  #ifdef DEBUG
    fprintf(stderr,"srcTextPath %s func %s\n",srcTextPath,__func__);
  #endif
  //Compares all the words in the source text path against a dictionary of words.
  //For each word in the source text, a singly linked list of possible matches 
  //from the dictionary is produced.
  //A collective list of possible syntactically correct words is produced and 
  //written to the outpath whose file pointer is 'words_learnt_ifp'

  char *dictPath = "wordlist.txt";
  FILE *dictFP = fopen(dictPath, "r");

  //File to be corrected
  FILE *srcfp = fopen(srcTextPath,"r");

  //Path where any found words that might be variants of dictionary-based words
  //will be written
  FILE *words_learnt_ifp = fopen(LEARNT_WORDS_PATH,"r+w");
  if (words_learnt_ifp == NULL){
   words_learnt_ifp = fopen(LEARNT_WORDS_PATH,"w");
  } 
  Node *storage = NULL;

  while (! feof(srcfp)){
    char *srcWord = getWord(srcfp);
    if (srcWord == NULL) continue;

    //Word Comparison will be done in lower case
    toLower(srcWord);

    #ifdef DEBUG
      fprintf(stderr,"srcWord %s\n",srcWord);
    #endif

    //Definition of function 'loadWord(...)'
    //Node *loadWord(FILE *,Node *,char *query,Bool LEN_MATCH_BOOL, Bool FIRST_LETTER_MATCH);

    //Therefore: Searching for words that have the same letter length, and same first letter
    //Add to 'storage' those words that have a ranked similarity to the word 
    //under scrutiny
    storage  = loadWord(dictFP, storage, srcWord, False, False);

    if (srcWord != NULL)
      free(srcWord);
  }

  //Time to write to memory words that had a high match percentage

  fprintf(words_learnt_ifp,"#Words learnt from examining file %s\n",
	  srcTextPath);

  if( serializeNode(storage,words_learnt_ifp) == True ){
    fprintf(stderr,"\033[32mWrote the learnt words to file \"%s\"\033[00m",
	  LEARNT_WORDS_PATH);
  }

  //And give unto OS, what belongs to OS -- release memory
  if (storage != NULL)
    nodeFree(storage);

  fprintf(words_learnt_ifp,"%c",EOF); 

  fclose(dictFP);
  fclose(srcfp);
  fclose(words_learnt_ifp);
}
