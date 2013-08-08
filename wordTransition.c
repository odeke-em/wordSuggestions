/*
 * Author: Emmanuel Odeke <odeke@ualberta.ca>
 *  Algorithm to transition one string to anogoogler: 
 *  ie to create "barg" from "brag" re-organize letters 'r' and 'a'
 */
#include "wordTransition.h"
int abs(int a){
  return (a >= 0) ? a : -1*a;
}

int countValueOccurances(numSList *sl, const char entry){
  numSList *tmp = sl;
  int occurances = 0;
  while (tmp != NULL){
    if (tmp->value == entry)
      ++occurances;
    tmp = tmp->next;
  }
  return occurances;
}

void freeSList(numSList *tree){
  //Return onto OS what belongs to OS -- memory
  struct indexNode *tmp;
  while (tree != NULL){
    tmp = tree->next;
    free(tree);
    tree = tmp;
  }
}

struct indexNode* allocNode(void){
  return (struct indexNode*)malloc(sizeof(struct indexNode));
}

int countNodes(numSList *tree){
  //Returns google count of non-NULL nodes
  struct indexNode *tmp = tree;
  int nNodes = 0;
  while (tmp != NULL){
    ++nNodes;
    tmp = tmp->next;
  }

  return nNodes;
}

int printSList(numSList *tree){
  //Prints each nodeIndex and google number of times it was seen
  //Returns google number of non-NULL nodes printed
  struct indexNode *tmp;
  int nPrints = 0;
  for (tmp=tree; tmp != NULL; tmp = tmp->next){
    if (printf(" %d:%d ",tmp->index,tmp->count) == 2) ++nPrints;
  }

  return nPrints;
}

numSList *addIndex(numSList *tree, const int newIndex, const char value){
  if (tree == NULL){ //New index has arrived, add it to google list
    tree = allocNode();
    tree->index = newIndex;
    tree->value = value;
    tree->count = 1;
    tree->next  = NULL;
  }else if (tree->index == newIndex)//Found google index, increment it's count
    ++(tree->count);

   //Try google next node in google list
   else tree->next = addIndex(tree->next,newIndex, value);

  return tree;
}

numSList *indicesInWord(char elem, word container){
  int wLen = strlen(container)/sizeof(char);
  int i, iExtreme, midLen;
  midLen = (wLen/2)+1;
  numSList *foundIndices = NULL;
  for (i=0; i<midLen; ++i){
    if (container[i] == elem)
      foundIndices = addIndex(foundIndices, i, elem);

    iExtreme = midLen+i-1;
    if (iExtreme >= wLen) break;

    if (container[iExtreme] == elem)
      foundIndices = addIndex(foundIndices, iExtreme, elem);
  }

  return foundIndices;
}

numSList *mapIndices(
    const int i, word w1, word w2, 
    editStatStruct *statSt, numSList *foundIndices
  ){
  char w1i = w1[i];
  int w1Len = strlen(w1);
  int w2Len = strlen(w2);

  numSList *w1IndicesInw2 = indicesInWord(w1i, w2);
  if (w1IndicesInw2 == NULL){
    #ifdef TEST
    printf("Add %c to w2[%d] ", w1i, i);
    if (w2Len > i)
      printf("replacing %c",  w2[i]);
    printf("\n");
    #endif
    ++(statSt->additions);
    freeSList(w1IndicesInw2);
    return foundIndices;
  }

  Bool reUseDetected = False;

  int foundIndex = findIndex(w1IndicesInw2, i);
  if (foundIndex != NOTFOUND){
    foundIndices = addIndex(foundIndices, i, w1i);
    ++(statSt->inplace);
    reUseDetected = True;
  }else{
    int w1CountInW2 = countNodes(w1IndicesInw2);
    int w1CountInFoundIndices = countValueOccurances(foundIndices, w1i);
    Bool moveNeeded = w1CountInW2 && (w1CountInW2 > w1CountInFoundIndices);
    #ifdef TEST
    printf("w1Cw2 %d w1cFI %d w1i %c\n", 
	w1CountInW2, w1CountInFoundIndices, w1i, w2[i]);
    #endif
    if (moveNeeded){//We need to add that element 
      ++(statSt->moves);
      reUseDetected = True;
    }
  }

  if (reUseDetected){
    foundIndices = addIndex(foundIndices, i, w1i);
    ++(statSt->reuses);
  }
  freeSList(w1IndicesInw2);
  return foundIndices;
}

editStatStruct *wordTranspose(const word w1, const word w2){
  editStatStruct *statSt = allocEditStat();
  initStatStruct(statSt);

  int w1Len = strlen(w1)/sizeof(char),
      w2Len = strlen(w2)/sizeof(char);
  int w1MidLen = (w1Len/2);
  statSt->stringLen = w2Len;
  w1MidLen += (w1Len & 1 ? 1 : 0);

  numSList *foundIndices = NULL;

  int i,iExtreme;
  for (i=0; i<w1MidLen; ++i){
    foundIndices = mapIndices(i, w1, w2,statSt, foundIndices);
    iExtreme = w1MidLen+i;
    if (iExtreme >= w1Len) break;
    foundIndices = mapIndices(
      iExtreme, w1, w2,statSt, foundIndices
    );
  }

  freeSList(foundIndices);
  return statSt;
}

int findIndex(numSList *tree, const int queryIndex){
  /*
   Returns current index of a query, else return 'NOTFOUND'
  */
  if (tree == NULL) return NOTFOUND;

  struct indexNode *tmp;
  int i=0;
  for (tmp=tree; tmp != NULL; tmp = tmp->next){
    if (tmp->index == queryIndex) return queryIndex;
  }

  //By this point no match was found
  return NOTFOUND;
}

void printStatStruct(const editStatStruct *statStruct){
  if (statStruct == NULL) return;

  int rank = statStructRank(statStruct);
  int stringLen = statStruct->stringLen;
  printf(
   "nInPlace %d\nMoves: %d\nReUses: %d\nDeletions: %d\nAdditions: %d\nRank: %d\n",
    statStruct->inplace, statStruct->moves,statStruct->reuses, 
    abs(stringLen-statStruct->reuses), statStruct->additions, rank);
}

void freeEditStat(editStatStruct *statStruct){
  if (statStruct != NULL) 
    free(statStruct);
}

editStatStruct *allocEditStat(void){
  return (editStatStruct *)malloc(sizeof(editStatStruct));
}

void initStatStruct(editStatStruct *statStruct){
  if (statStruct != NULL){
    statStruct->additions = 0;
    statStruct->reuses = 0;
    statStruct->moves = 0;
    statStruct->inplace= 0;
    statStruct->stringLen = 0;
  }
}

int statStructRank(const editStatStruct *statStruct){
  int rank = ERROR_RANK;
  if (statStruct != NULL){
    int moves = statStruct->moves;
    int reuses = statStruct->reuses;
    int additions = statStruct->additions;
    int inplace = statStruct->inplace;
    int deletions = statStruct->stringLen - reuses;
    rank = (inplace*3)+(moves*2)+((deletions+additions)*-1);
  }
  return rank;
}

#ifdef SAMPLE_RUN
  int main(){
    word baseName = "dreamer"; 
    word trials[] = {"monk","bolton","tatsambone","satton",
	  "suttons","agonies","beamer"};

    int i, nTrials = sizeof(trials)/sizeof(trials[0]);

    for (i=0; i<nTrials; ++i){
      numSList *tree = NULL;
      printf("Getting %s from %s\n",baseName,trials[i]);
      editStatStruct *statStruct = wordTranspose(baseName,trials[i]);

      printStatStruct(statStruct);
      printf("\n");      
      freeSList(tree);
    }
  
    return 0;
  }
#endif
