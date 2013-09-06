#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../include/wordLib.h"
#include "../include/hashLib.h"

HashComparison hashComp(const hash subject, const hash query){
  if (subject == query) return HASH_EQ;
  else if (subject < query) return HASH_LT;
  return HASH_GT;
}

//Really bad hash function, just for testing purposes for now
hash hashFunc(const word w){
  if (w == NULL) return UNDEF_HASH_VALUE;
  int wLen = strlen(w)/sizeof(char);

  hash hValue = UNDEF_HASH_VALUE;
  int i;

  for (i=0; i<wLen; ++i){
    hValue += (i*(w[i])+(1<<(i%32)));
  }

  return hValue;
}

void addHash(hashList *hL, const word query){
  if (hL == NULL || query == NULL){
    raiseWarning("First allocate space for the hashList container");
    return;
  }

  if (hL->hEArray == NULL){
    hL->arrSize = INIT_N_HASH_ELEMS;
    hL->hEArray = (hashElem *)malloc(sizeof(hashElem)*(hL->arrSize));
    hL->currentIndex = 0;
  }else{
    hash queryHash = hashFunc(query);
    if (hFind(hL, queryHash) == True) return;
  }

  if (hL->currentIndex >= hL->arrSize){
    hL->arrSize *= 2;
    hL->hEArray = (hashElem *)realloc(
      hL->hEArray, sizeof(hashElem)*(hL->arrSize)
    );
  }

  uint32 currentIndex = hL->currentIndex;

  setHash(&(hL->hEArray[currentIndex]), query);
  ++(hL->currentIndex);
}

Bool hFind(hashList *hL, const hash queryHash){
  if (hL == NULL || hL->hEArray == NULL) return False;

  hashElem *hArray = hL->hEArray;
  uint32 index=0, maxIndex = hL->currentIndex;
  while(index < maxIndex){
    hashElem tmpElem = hArray[index];
    //printf("qH %d ", queryHash, printHashElem(&tmpElem)); 
    if (hashComp(tmpElem.hashValue, queryHash) == HASH_EQ) return True;
    ++index;
  }

  return False;
}

hashElem *allocHashElem(void){
  return (hashElem *)malloc(sizeof(hashElem));
}

void initHashElem(hashElem *hE){
  if (hE == NULL){
    hE = allocHashElem();
    assert(hE);
  }
  hE->hashValue = INIT_HASH_VALUE;
  hE->wordValue = NULL;
}

int printHashElem(const hashElem *hE){
  if (hE == NULL||hE->wordValue == NULL){
    printf("{ w:? h:? }"); 
    return -1;
  }

  return printf("{ w:%s h:%ld }", hE->wordValue, hE->hashValue);
}

void setHash(hashElem *hE, const word w){
  if (hE == NULL) return;
  hE->wordValue = strdup(w);
  hE->hashValue = hashFunc(w);
}

void freeHashElemPtr(hashElem *hE){
  if (hE == NULL) return;
  else if (hE->wordValue != NULL) freeWord(hE->wordValue);

  free(hE);
}

void freeHashElem(hashElem hE){
  if (hE.wordValue != NULL) freeWord(hE.wordValue);
}

int printHashList(hashList *hL){
  if (hL == NULL || hL->hEArray == NULL) return -1;
  int i=0, end=hL->currentIndex;
  for (i=0; i<end; ++i){
    printHashElem(&(hL->hEArray[i]));
  }
  printf("\n");

  return i;
}

hashList *fileToHashList(const word filePath){
  if (filePath == NULL) return NULL;
  FILE *ifp = fopen(filePath, "r");

  if (ifp == NULL){
    raiseWarning("Could not open in file");
    return NULL;
  }

  hashList *hL = (hashList *)malloc(sizeof(hashList));
  while (! feof(ifp)){
    word wordIn = getWord(ifp);
    addHash(hL, wordIn);
    freeWord(wordIn);
  }

  return hL;    
}

void freeHashList(hashList *hL){
  if (hL == NULL || hL->hEArray == NULL) return;
  int i, maxElems = hL->arrSize;

  for (i=0; i<maxElems; ++i){
    freeHashElem(hL->hEArray[i]);
  }
}

hashElem *hSearch(const hashList *hL, const word query){
  if (hL == NULL || hL->arrSize == 0) return NULL;

  hash queryHash = hashFunc(query);

  printf("queryHash %ld\n", queryHash);
  int start=0, end=hL->arrSize-1;
  int mid;

  while (start <= end){
    hashElem *tmp = &(hL->hEArray[start]);
    if (hashComp(tmp->hashValue, queryHash) == HASH_EQ) return tmp;

    tmp = &(hL->hEArray[end]);
    if (hashComp(tmp->hashValue, queryHash) == HASH_EQ) return tmp;

    mid = (start+end)/2;
    tmp = &(hL->hEArray[mid]);
    
    int midComparison = hashComp(queryHash, tmp->hashValue);
    if (midComparison == HASH_EQ) return tmp;
  
    else if (midComparison == HASH_GT){
      start = mid-1; 
      end -= 1; 
    }else{
      end = mid-1; 
      start += 1; 
    }
  }

  return NULL;
}

void swap(void *a, void *b){
  void *tmp = a;
  a = b;
  b = a;
}

HashComparison hashElemComp(const hashElem *hA, const hashElem *hB){
  if (hA == NULL) return HASH_LT;
  if (hB == NULL) return HASH_GT;

  return hashComp(hA->hashValue, hB->hashValue);
}

hashList *hMergeSort(hashList *hL){
  if (hL == NULL || hL->currentIndex == 0) return hL;
  else if (hL->currentIndex == 1){
    hashElem *hA = &(hL->hEArray[0]);
    hashElem *hB = &(hL->hEArray[1]);
    HashComparison hComp = hashElemComp(hA, hB); 

    if (hComp == HASH_GT)  swap(hA, hB);
    return hL;
  }

  uint32 mid = (hL->currentIndex)/2;

  hashList *left  = (hashList *)malloc(sizeof(hashList));
  hashList *right = (hashList *)malloc(sizeof(hashList));

  left->hEArray = NULL;
  right->hEArray = NULL;

  int iL=0, iR=mid-1;
  while (iL < mid){
    hashElem tmpElem = hL->hEArray[iL];
    addHash(left, tmpElem.wordValue);

    if (iR >= hL->currentIndex) break;

    tmpElem = hL->hEArray[iR];
    addHash(right, tmpElem.wordValue);
    ++iR;

    ++iL;
  }

  return hMerge(hMergeSort(left), hMergeSort(right));
}

hashList *hMerge(hashList *left, hashList *right){
  if (left == NULL) return right;
  if (right == NULL) return left;

  hashList *hCombo = (hashList *)malloc(sizeof(hashList));
  hCombo->hEArray = NULL;

  int i, iEnd;
  int j, jEnd;

  i = 0;
  j = 0;
  iEnd = left->currentIndex;
  jEnd = right->currentIndex;

  while ((i < iEnd) && (j < jEnd)){
    hashElem tmpL = left->hEArray[i];
    hashElem tmpR = right->hEArray[j];

    HashComparison comp = hashElemComp(&tmpL, &tmpR);
    switch(comp){
      case HASH_LT:{
	addHash(hCombo, tmpL.wordValue);
	++i;
	break;
      }

      case HASH_GT:{
	addHash(hCombo, tmpR.wordValue);
	++j;
	break;
      }

      default:{
	addHash(hCombo, tmpL.wordValue);
	++i;
	addHash(hCombo, tmpR.wordValue);
	++j;
      }
    }
  }

  while (i < iEnd){
    hashElem tmpL = left->hEArray[i];
    addHash(hCombo, tmpL.wordValue);
    ++i;
  }

  while (j < jEnd){
    hashElem tmpR = right->hEArray[j];
    addHash(hCombo, tmpR.wordValue);
    ++j;
  }
   
  freeHashList(left); 
  freeHashList(right); 
  return hCombo;
}

int main(){
  hashList *hL = fileToHashList("src/hashLib.c");

  hashList *merged = hMergeSort(hL);
  word query = "odrex";
  hashElem *foundIndex = hSearch(merged, query);
  printHashList(hL);
  if (foundIndex == NULL)  printHashElem(foundIndex);
  else printf("%s not found\n", query);

  printf("query %s in hL %s\n", query, (hFind(hL, hashFunc(query)) == True ? "true" : "false"));
  freeHashList(merged);
  freeHashList(hL);
  return 0;
}
