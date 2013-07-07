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

  #define NOTFOUND -1

  int abs(int );

  //Singly linked list to store indices of found letters
  typedef struct indexNode{
    int index;
    int count;
    struct indexNode *next;
  }numSList;

  //Frees the singly-linked list
  void freeSList(numSList *tree);

  //Requests for a node's worth of memory
  struct indexNode* allocNode(void);

  //Returns the count of non-NULL indexNodes
  int countNodes(numSList *tree);

  //Prints each indexNodeIndex and the number of times it was seen
  //Returns the number of non-NULL indexNodes printed
  int printSList(numSList *tree);

  //Add indices to the list or increment their visit count
  numSList *addIndex(numSList *tree,int newIndex);

  //Returns the index of a query, else return 'NOTFOUND'
  int findIndex(numSList *tree, int queryIndex);

  //Prints step by step modifications in order to re-create the base string 'w1'
  //from the subject string 'w2'. Stores information on the number of chars to
  //be added, removed or added to accomplish the mission
  numSList *wordTransition( const char *w1, int w1Index, const char *w2,
    numSList *foundIndices, int *nAdditions, int *nMoves, int *nReUsable);
#endif
