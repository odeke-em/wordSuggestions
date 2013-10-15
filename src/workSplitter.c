/*
  Author: Emmanuel Odeke <odeke@ualberta.ca
   Module to enable fragmenting of file into almost equal parts. 
   For each divided part, a file is created and it's content is written.
   This enables distributed autoCorrection of the various fragments,
   that can be later on be re-combined
*/
#include <pthread.h>

#include "../include/Node.h"
#include "../include/errors.h"
#include "../include/constants.h"
#include "../include/wordSearch.h"
#include "../include/workSplitter.h"

void navFree(navigator *nav) {
  if (nav == NULL) return;
  freeWord(nav->toPath);
}

void *cat(void *data) {
  //Given a navigator, copy the data from the infile's
  //start to end positions to the outfile
  navigator *nav =(navigator *)data;

  int *nBytes = (int *)malloc(sizeof(int));
  *nBytes = EOF;

  if ((nav == NULL) || (nav->srcfp == NULL)) return nBytes;

  word dest = nav->toPath;
  FILE *destfp = fopen(dest, "w"); 
  if (destfp == NULL) return nBytes; 

  int i=nav->start, end=nav->end;
  if (i >= end) return nBytes;

  int originalPosition = ftell(nav->srcfp);
  fseek(nav->srcfp, i, SEEK_SET);

  char c;
  FILE *fp = nav->srcfp;
  while (i < end){
   c = getc(fp); 
   if (c == EOF) break;

    fputc(c, destfp);
    ++i;
  }

  //Move the reader back to its original position
  fseek(nav->srcfp, originalPosition, SEEK_SET);
  fclose(destfp);

  return nBytes;
}

void printNavigator(navigator *nav) {
  if (nav == NULL) return;

  printf("start %d end %d fSize %d\n", nav->start, nav->end, nav->fileSize);
}

navigatorList *navListAlloc(void) {
  navigatorList *navList = (navigatorList *)malloc(sizeof(navigatorList));

  return navList;
}

size_t getFileSize(FILE *fp) {
  if (fp == NULL) return EOF;

  const size_t originalPosition = ftell(fp);

  //Moving read head to the end of the file
  fseek(fp, 0L, SEEK_END);

  const size_t fSize = ftell(fp);
  
  //Moving back to the original position
  fseek(fp, originalPosition, SEEK_SET);

  return fSize;
}

void initNavList(navigatorList *navContainer, const int *n) {
  if (navContainer == NULL){
    fprintf(
      stderr, "Error: Null navigatorlist passed in for initialization\n"
    );
    exit(-2);
  }
  navContainer->srcfp = NULL;
  navContainer->nPartitions = *n;
  navContainer->navList = (navigator *)malloc(sizeof(navigator)*(*n));
}

void navListFree(navigatorList *navL) {
  if (navL == NULL) return;
  int i;
  for (i=0; i<navL->nPartitions; ++i){
    navFree(&(navL->navList[i]));
  }
  if (navL->navList != NULL) free(navL->navList);
  free(navL);
}

void initNavigator(navigator *nav) {
  if (nav == NULL){
    fprintf(
      stderr, "Error: Null navigator struct passed in for initialization\n"
    );
    exit(-2);
  }
  nav->start = 0;
  nav->end   = 0;
  nav->srcfp = NULL;
  nav->toPath = NULL;
  nav->fileSize = 0;
}

void setNavigator(
  navigator *nav, FILE *tfp, const int *start, const int *end, const word path
  ) {
  if (nav == NULL) {
    fprintf(stderr, "Null navigator struct passed in %s\n", __func__);
  }

  nav->srcfp = tfp;
  nav->start = *start;
  nav->end   = *end;
  nav->fileSize = getFileSize(tfp);
  nav->toPath = strdup(path);
}

int getFragmentSize(const navigator *nav) {
  if (nav == NULL) return -1;

  return nav->end - nav->start;
}

navigatorList *fragmentFile(FILE *tfp, const int *nPartitions) {
  if (tfp == NULL){
    fprintf(stderr, "Null file pointer passed in for partitioning\n");
    return NULL;
  }

  if ((nPartitions == NULL) || (*nPartitions < 0)){
    raiseWarning(
      "Only non-NULL and positive numbers of partition allowed"
    );
    return NULL;
  }

  int fSize = getFileSize(tfp)/1;
  navigatorList *navContainer = navListAlloc();
  initNavList(navContainer, nPartitions);
 
  int aveChunckSize = (fSize/(*nPartitions)); 
  int start=0, end, i=0;
  while (i < *nPartitions){
    //For each partition, we'll have a start and end position
    //Move the file pointer until the current 'end'. From there
    // then move until the next non-space token is found
    end = start+aveChunckSize;
    //printf("ostart %d oend %d\n", start, end);
    if (end > fSize){
      end = fSize;
    }

    fseek(tfp, end, SEEK_SET);
    skipTillCondition(tfp, isspace);

    end = ftell(tfp);
    word allocatedPath = newWord(5);
    sprintf(allocatedPath, "txt%d", i);
    setNavigator(&(navContainer->navList[i]), tfp, &start, &end, allocatedPath);
    start = end;

    freeWord(allocatedPath);
    ++i;
  } 

  navContainer->nPartitions = i;
  return navContainer; 
}

int main(int argc, word argv[]) {
  if (argc != 3){
    fprintf(stderr,"Usage:\033[33m <filePath> <numberOfThreads>\033[00m\n");
    exit(-2);
  }

  int n;
  if (sscanf(argv[2],"%d", &n) != 1){
    raiseWarning(
      "Failed to parse the number of threads as an integer [arg 2]"
    );
    exit(-2);
  }

  const word sourcePath = argv[1];

  FILE *ifp = fopen(sourcePath, "r");
  if (ifp == NULL) {
    raiseWarning("Invalid input file");
    exit(-3);
  }

  wordArrayStruct *wArrSt = wordsInFile(DICTIONARY_PATH);
  navigatorList *navL = fragmentFile(ifp, &n);

  if (navL == NULL) {
    exit(-4);
  }

  int nParts = navL->nPartitions;

  printf(
    "Splitting the file \033[33m%s\033[00m of %zdbytes into %d chunks of which\n", 
    sourcePath, getFileSize(ifp), nParts
  );

  pthread_t storage_th[nParts];
  
  int i;
  //Fragmentation of file going on here
  for (i=0; i<nParts; ++i){
    navigator *navPtr = &(navL->navList[i]);
    printf("\tPartition %d :: %d bytes\n", i+1, getFragmentSize(navPtr));
    pthread_create(&(storage_th[i]), NULL, cat, navPtr);
  }

  printf("\n");

  for (i=0; i<nParts; ++i){
    pthread_join(storage_th[i], NULL);
  }

  //Now autoCorrection of each fragmented file
  for (i=0; i<nParts; ++i){
    navigator *navPtr = &(navL->navList[i]);
    navPtr->dictWArrayStruct = wArrSt;
    printf("Now autoCorrecting partition %d on thread %d\n", i+1, i+1);
    pthread_create(&(storage_th[i]), NULL, autoC, navPtr);
  }

  for (i=0; i<nParts; ++i){
    pthread_join(storage_th[i], NULL);

    printf("\033[32mThread %d returned\033[00m\n", i+1);
  }

  fclose(ifp);

  freeWordArrayStruct(wArrSt);
  navListFree(navL);
  return 0;
}

void *autoC(void *data) {
  navigator *nav = (navigator *)data;
  word srcPath = nav->toPath;
  int pathLen = strlen(srcPath)/sizeof(char);

  #ifdef DEBUG
    fprintf(stderr,"srcPath %s func %s\n",srcPath,__func__);
  #endif

  word learntPath = newWord(pathLen+1);
  word correctedPath = newWord(pathLen+1);

  sprintf(learntPath, "%sL", srcPath);
  sprintf(correctedPath, "%sC", srcPath);

  FILE *srcfp = fopen(srcPath,"r");
  FILE *words_learnt_ifp = fopen(learntPath,"w");
  FILE *correctedfp = fopen(correctedPath,"w");

  wordArrayStruct *dictWArrStruct = nav->dictWArrayStruct;
  if (words_learnt_ifp == NULL) {
   words_learnt_ifp = fopen(learntPath,"w");
  } 

  Node *storage = NULL;

  while (! feof(srcfp)) {
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

    storage = getSuggestions(
      dictWArrStruct, correctedfp, storage, srcWord, matchCriteria
    );

    if (srcWord != NULL)  free(srcWord);
  }

  //Time to write to memory matched words
  fprintf(
    words_learnt_ifp,"#Words learnt from examining file %s\n", srcPath
  );

  if (serializeNode(storage,words_learnt_ifp) == True) {
    fprintf(
      stderr,"\033[32mWrote the learnt words to file \"%s\"\033[00m\n", 
      learntPath
    );
  }

  //And give unto OS, what belongs to OS -- release memory
  if (storage != NULL) nodeFree(storage);

  free(learntPath);
  free(correctedPath);

  fclose(srcfp);
  fclose(correctedfp);
  fclose(words_learnt_ifp);

  return NULL;
}
