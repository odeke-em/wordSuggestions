#include <pthread.h>
#include "workSplitter.h"

int cat(const navigator *nav, const char *dest){
  if ((nav == NULL) || (nav->fp == NULL)) return EOF;

  FILE *destfp = fopen(dest, "w"); 
  if (destfp == NULL) return EOF; 

  int i=nav->start, end=nav->end;
  if (i >= end) return EOF;

  int originalPosition = ftell(nav->fp);
  fseek(nav->fp, i, SEEK_SET);

  char c;
  FILE *fp = nav->fp;
  while (i < end){
   c = getc(fp); 
   if (c == EOF) break;

    fputc(c, destfp);
    ++i;
  }

  //Move the reader back to its original position
  fseek(nav->fp, originalPosition, SEEK_SET);
  fclose(destfp);
  return i;
}

void printNavigator(navigator *nav){
  if (nav == NULL) return;

  printf("start %d end %d fSize %d\n", nav->start, nav->end, nav->fileSize);
}

navigatorList *navListAlloc(void){
  navigatorList *navList = (navigatorList *)malloc(sizeof(navigatorList));

  return navList;
}

size_t fileSize(FILE *fp){
  if (fp == NULL) return EOF;

  const size_t originalPosition = ftell(fp);

  //Moving read head to the end of the file
  fseek(fp, 0L, SEEK_END);

  const size_t fSize = ftell(fp);
  
  //Moving back to the original position
  fseek(fp, originalPosition, SEEK_SET);

  return fSize;
}

void initNavList(navigatorList *navContainer, const FILE *fp, const int *n){
  if (navContainer == NULL){
    fprintf(
      stderr, "Error: Null navigatorlist passed in for initialization\n"
    );
    exit(-2);
  }

  navContainer->nPartitions = *n;
  navContainer->navList = (navigator *)malloc(sizeof(navigator)*(*n));
}

void navListFree(navigatorList *navL){
  if (navL == NULL) return;

  if (navL->navList != NULL) free(navL->navList);
  free(navL);
}

void initNavigator(navigator *nav){
  if (nav == NULL){
    fprintf(
      stderr, "Error: Null navigator struct passed in for initialization\n"
    );
    exit(-2);
  }
  nav->start = 0;
  nav->end   = 0;
  nav->fp    = NULL;
  nav->fileSize = 0;
}

void setNavigator(navigator *nav, FILE *tfp, const int *start, const int *end){
  if (nav == NULL){
    fprintf(stderr, "Null navigator struct passed in %s\n", __func__);
  }

  nav->fp = tfp;
  nav->start = *start;
  nav->end   = *end;
  nav->fileSize = fileSize(tfp);
}

navigatorList *partitionFile(FILE *tfp, const int *nPartitions){
  if (tfp == NULL){
    fprintf(stderr, "Null file pointer passed in for partitioning\n");
    return NULL;
  }

  if ((nPartitions == NULL) || (*nPartitions < 0)){
    fprintf(
      stderr, "Only non-NULL and positive numbers of partition allowed\n"
    );
    return NULL;
  }

  size_t fSize = fileSize(tfp);
  navigatorList *navContainer = navListAlloc();
  initNavList(navContainer, tfp, nPartitions);
 
  int aveChunckSize = (fSize/(*nPartitions)); 
  int start=0, end, i=0;
  while (i < *nPartitions){
    //Move the file pointer until the current end then move until 
    //the next non-space token is found
    end = start+aveChunckSize;
    //printf("ostart %d oend %d\n", start, end);
    if (end > fSize){
      end = fSize;
    }

    fseek(tfp, end, SEEK_SET);
    int nSkips = skipTillCondition(tfp, isspace);

    end = ftell(tfp);
    //printf("start %d end %d nSkips %d\n", start, end, nSkips);

    setNavigator(&(navContainer->navList[i]), tfp, &start, &end);
    start = end;
    ++i;
  } 

  navContainer->nPartitions = i;
  return navContainer; 
}

int main(int argc, char *argv[]){
  if (argc != 3){
    fprintf(stderr,"Usage <filePath> <n_threads>\n");
    exit(-2);
  }

  int n;
  if (sscanf(argv[2],"%d", &n) != 1){
    fprintf(stderr,"Failed to parse an integer as argument 2\n");
    exit(-2);
  }

  navigator nav;
  initNavigator(&nav);

  FILE *ifp = fopen(argv[1], "r");
  size_t fSize = fileSize(ifp);

  navigatorList *navL = partitionFile(ifp, &n);

  int nParts = navL->nPartitions;
  int i;

  for (i=0; i<nParts; ++i){
    char *sp = (char *)malloc(sizeof(char)*4);
    sprintf(sp, "txt%d", i);
    printNavigator(&(navL->navList[i]));
    cat(&(navL->navList[i]), sp);
    free(sp);
  }

  fclose(ifp);
  navListFree(navL);
  return 0;
}
