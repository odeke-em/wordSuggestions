/*
 * Author: Emmanuel Odeke <odeke@ualberta.ca>
 *  Algorithm to transition one string to another: 
 *  ie to create "barg" from "brag" re-organize letters 'r' and 'a'
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "wordTransition.h"

int abs(int a){
  return (a >= 0) ? a : -1*a;
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
  //Returns the count of non-NULL nodes
  struct indexNode *tmp = tree;
  int nNodes = 0;
  while (tmp != NULL){
    ++nNodes;
    tmp = tmp->next;
  }

  return nNodes;
}

int printSList(numSList *tree){
  //Prints each nodeIndex and the number of times it was seen
  //Returns the number of non-NULL nodes printed
  struct indexNode *tmp;
  int nPrints = 0;
  for (tmp=tree; tmp != NULL; tmp = tmp->next){
    if (printf(" %d:%d ",tmp->index,tmp->count) == 2) ++nPrints;
  }

  return nPrints;
}

numSList *addIndex(numSList *tree,int newIndex){
  if (tree == NULL){ //New index has arrived, add it to the list
    tree = allocNode();
    tree->index = newIndex;
    tree->count = 1;
    tree->next  = NULL;
  }else if (tree->index == newIndex) //Found the index, increment it's count
    ++(tree->count);

   //Try the next node in the list
   else tree->next = addIndex(tree->next,newIndex);

  return tree;
}

int findIndex(numSList *tree, int queryIndex){
  /*
   Returns the index of a query, else return 'NOTFOUND'
  */
  if (tree == NULL) return NOTFOUND;

  struct indexNode *tmp;
  int i=0;
  for (tmp=tree; tmp != NULL; tmp = tmp->next){
    if (tmp->index == queryIndex) return i;
  }

  //By this point no match was found
  return NOTFOUND;
}

numSList *wordTransition( const char *w1, int w1Index, const char *w2,
  numSList *foundIndices, editStatStruct *statStruct){ // int *nAdditions, int *nMoves, int *nReUsable){
  /*
    Prints step by step modifications in order to re-create the base string 'w1'
    from the subject string 'w2'
  */
  if ((w1 == NULL) || (w2 == NULL) || (statStruct == NULL)) return foundIndices;

  if (w1Index >= strlen(w1)){ 
    statStruct->stringLen = strlen(w2);
    return foundIndices; 
  }

  char cW1 = w1[w1Index];
  int i=0,foundCh=0;

  for (i=0; i<strlen(w2); ++i){
    int alreadyVisited = findIndex(foundIndices, i);

    if (w2[i] == cW1){
      if (alreadyVisited == NOTFOUND){
        //Mark the index as now visited
        foundIndices = addIndex(foundIndices,i);
        if (i != w1Index){
	  #ifdef TEST
            printf("move w2[%d] %c to w2[%d]\n",i,w2[i],w1Index);
	  #endif
	  ++(statStruct->moves);
          //++*nMoves;
        }

	++(statStruct->reuses);
	//++*nReUsable;
        foundCh = 1;

        break;
      }
    } //Otherwise we need to add that character
  }
  if (foundCh != 1){
      #ifdef TEST
        printf("Replace w2[%d] %c with %c\n",w1Index,w2[w1Index],cW1);
      #endif
      ++(statStruct->additions);
      //++*nAdditions;
  }
  
  return wordTransition(w1, ++w1Index, w2,foundIndices, statStruct); 
}
void printStatStruct(const editStatStruct *statStruct){
  if (statStruct == NULL) return;

  int stringLen = statStruct->stringLen;
  printf("Moves: %d\nReUses: %d\nDeletions: %d\n",
    statStruct->moves,statStruct->reuses, 
    abs(stringLen-statStruct->reuses));
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
    statStruct->stringLen = 0;
  }
}

int statStructRank(editStatStruct *statStruct){
  int rank = ERROR_RANK;
  if (statStruct != NULL){
    int moves = statStruct->moves;
    int reuses = statStruct->reuses;
    int additions = statStruct->additions;
    int deletions = statStruct->stringLen - reuses;

    rank = (additions*-2)+(reuses)+((deletions+moves)*-1);
    //printf("rank %d\n", rank);
  }
  return rank;
}

#ifdef SAMPLE_RUN
  int main(){
    char *baseName = "morp"; 
    char *trials[] = {"monk","brag","tatsambone","satton",
	  "suttons","konrad","morp"};

    int i, nTrials = sizeof(trials)/sizeof(trials[0]);

    editStatStruct statStruct;
    for (i=0; i<nTrials; ++i){
      numSList *tree = NULL;
      *nTransitions = 0, *nMoves=0, *nReUsable=0;
      printf("Getting %s from %s\n",baseName,trials[i]);

      initStatStruct(&statStruct);
      tree = wordTransition(baseName,0,trials[i],tree, &statStruct);

      printStatStruct(&statStruct);
      
      //printf("Rank %d\n", statStructRank(&statStruct));
     
      freeSList(tree);
    }
  
    free(nMoves);
    free(nTransitions);
    free(nReUsable);

    return 0;
  }
#endif
