/*
 * Author: Emmanuel Odeke <odeke@ualberta.ca>
 *  Algorithm to transition one string to another: 
 *  ie to create "barg" from "brag" re-organize letters 'r' and 'a'
 */
#ifndef _WORD_TRANSITION_H
#define _WORD_TRANSITION_H
  #include <stdio.h>
  #include <string.h>
  #include <stdlib.h>
  #include "customTypes.h"

  #define NOTFOUND -1
  #define ERROR_RANK -0xffff //Arbitrary value here

  int abs(int);

  //Frees the singly-linked list
  void freeSList(numSList *);

  //Requests for a node's worth of memory
  struct indexNode* allocNode(void);

  void freeEditStat(editStatStruct *);

  editStatStruct *allocEditStat(void);

  //Returns the 'rank' from that statistics stored in the statStruct
  //Recipe for rank:
  //  rank = (additions*-2)+(reuses)+((deletions+moves)*-1)
  int statStructRank(const editStatStruct *);

  void printStatStruct(const editStatStruct *);

  //Initializes the 'editStatStruct'
  void initStatStruct(editStatStruct *);

  //Returns the count of non-NULL indexNodes
  int countNodes(numSList *);

  //Prints each indexNodeIndex and the number of times it was seen
  //Returns the number of non-NULL indexNodes printed
  int printSList(numSList *);

  //Add indices to the list or increment their visit count
  numSList *addIndex(numSList *, const int, const char);

  //Returns the index of a query, else return 'NOTFOUND'
  int findIndex(numSList *, const int);

  //Prints step by step modifications in order to re-create the base string 'w1'
  //from the subject string 'w2'. Stores information on the number of chars to
  //be added, removed or added to accomplish the mission
  editStatStruct *wordTranspose(const word, const word);

  int countValueOccurances(numSList *, const char);

  numSList *mapIndices(
    const int i, const word ,const word, editStatStruct *, numSList *
  );
#endif
