/*Author: Emmanuel Odeke <odeke@ualberta.ca>*/

#include <pthread.h>
#include "../include/wordSearch.h"
#include "../include/utilityFuncs.h"

void  autoCorrect(const wordArrayStruct *, FILE *, const word , const word , long *); 

static size_t MAX_PATH = 110;
static pthread_cond_t cond_t = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t main_tx = PTHREAD_MUTEX_INITIALIZER;

typedef struct{
  void (*func)(const wordArrayStruct *, FILE *, const word,const word, long *);
  word path;
  word learntPath;
  FILE *destFP;
  long *freaderPosition;
  wordArrayStruct *wordArraySt;
} funcStruct;


void *runFunc(void *data){
  funcStruct *f = (funcStruct *)data;
  pthread_mutex_lock(&main_tx);
  f->func(f->wordArraySt, f->destFP, f->path,f->learntPath, f->freaderPosition);
  pthread_cond_signal(&cond_t);

  pthread_mutex_unlock(&main_tx);
  return NULL;
}

struct procStruct{
  Bool *processDone;
  long *freaderPosition;
  long *fileSize;
};

void *timeScreen(void *data){
  struct procStruct *p = (struct procStruct *)data;
  int i=0;
  fprintf(stderr, "%c[2K",27);
  while (*(p->processDone) == False){
    fprintf(
      stderr,"Processed %ld/%ld bytes\r", *(p->freaderPosition), *(p->fileSize)
    );
    ++i;
    sleep(1);
  }
  //fprintf(stderr,"\n");
  fprintf(stdout,"\n %d second%c spent \n",i, (i != 1 ? 's' : ' '));
  return NULL;
}

int main(int argc, word argv[]){
  if (argc < 3){
    fprintf(stderr,
      "Usage: <srcFile> <storageForLearntPath> [optional correctedTxtPath]\n");
    exit(-1);
  }
  Bool doneReading = False, validfile;
  pthread_t timer_t;
  pthread_t main_th;
  
  word path= newWord(MAX_PATH);
  word learntPath= newWord(MAX_PATH);

  Bool *procDone = (Bool *)malloc(sizeof(Bool));

  long *freaderPosition = (long *)malloc(sizeof(long));
  long *fileSize = (long *)malloc(sizeof(long));

  funcStruct p;
  p.func= autoCorrect;

  struct procStruct procSt;
  procSt.processDone = procDone; 
  procSt.freaderPosition = freaderPosition;
  procSt.fileSize = fileSize;
  wordArrayStruct *wASt = NULL;
  while (! doneReading){
    if ((sscanf(argv[1],"%s",path) != 1) || (*path == EOF)){ 
      fprintf(stderr,"EOF encountered. Done reading\n");
      exit(0);
    };
    if (sscanf(argv[2],"%s",learntPath) != 1){
      fprintf(stderr,"Could not read in the learnt path\n");
      exit(-1);
    }
    FILE *correctedDest = NULL;

    if (argc == 4){
      //Time to fetch the path to write the corrected text to
      correctedDest = fopen(argv[3], "w");
    }

    validfile = isValidFile(path, fileSize);
    if(validfile){
      wASt = wordsInFile(DICTIONARY_PATH);
      *procDone = False;
      p.path = path;
      p.wordArraySt = wASt;
      p.learntPath = learntPath;
      p.freaderPosition = procSt.freaderPosition;
      p.destFP = correctedDest;
      pthread_create(&main_th, NULL, runFunc, &p);
      pthread_create(&timer_t, NULL, timeScreen, &procSt);
      pthread_cond_wait(&cond_t, &main_tx);

      *procDone = True;
      sleep(1);
      fprintf(stderr,"Done.\n");
    }else
      fprintf(stderr,"Non-existant path %s\n",path);
    #ifndef MULTIPLE_PROC
      break;
    #endif
  }
  free(procDone);
  free(freaderPosition);
  free(fileSize);

  freeWord(path);
  freeWord(learntPath);
  freeWordArrayStruct(wASt);

  pthread_mutex_destroy(&main_tx);
  pthread_cond_destroy(&cond_t);

  return 0;
}

void autoCorrect(
    const wordArrayStruct *wASt, FILE *correctedDest, const word srcTextPath, 
    const word learntPath, long *freaderPosition
  ){
  #ifdef DEBUG
    fprintf(stderr,"srcTextPath %s func %s\n",srcTextPath,__func__);
  #endif
  //Compares all words in the source text path against a dictionary of words.
  //For each word in the source text, a singly linked list of possible matches 
  //from the dictionary is produced.
  //A collective list of possible syntactically correct words is produced and 
  //written to the outpath whose file pointer is 'words_learnt_ifp'

  //File to be corrected
  FILE *srcfp = fopen(srcTextPath,"r");

  //Path where any found words that might be variants of dictionary-based words
  //will be written
  FILE *words_learnt_ifp = fopen(learntPath,"r+w");
  if (words_learnt_ifp == NULL){
   words_learnt_ifp = fopen(learntPath,"w");
  } 
  Node *storage = NULL;

  while (! feof(srcfp)){
    word srcWord = getWord(srcfp, isalpha);
    if (srcWord == NULL) continue;
    //Word Comparison will be done in lower case
    toLower(srcWord);

    #ifdef DEBUG
      fprintf(stderr,"srcWord %s\n",srcWord);
    #endif

    wordMatchCriteria matchCriteria;
    matchCriteria.lenMatch_bool = False; 
    matchCriteria.firstLetterMatch_bool = False; 

    storage = loadWord(wASt, correctedDest, storage, srcWord, matchCriteria);

    if (srcWord != NULL)
      free(srcWord);

    *freaderPosition= ftell(srcfp);
    //skipTillCondition(srcfp, notSpace);
  }

  //Time to write to memory matched words
  fprintf(
    words_learnt_ifp,"#Words learnt from examining file %s\n", srcTextPath
  );

  if(serializeNode(storage,words_learnt_ifp) == True){
    fprintf(
      stderr,"\033[32mWrote the learnt words to file \"%s\"\033[00m\n", learntPath
    );
  }

  nodeFree(storage);

  fclose(srcfp);
  fclose(words_learnt_ifp);
}
