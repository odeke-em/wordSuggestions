#ifndef _CUSTOM_TYPES_H
#define _CUSTOM_TYPES_H
  typedef char *word;

  typedef unsigned int uint32;

  typedef enum{
    Invalid=-1,
    False=0,
    True=1
  } Bool;

  //Singly linked list to store indices of found letters
  typedef struct indexNode{
    int index;
    int count;
    struct indexNode *next;
  }numSList;

  typedef struct{
    int additions;
    int moves;
    int reuses;
    int inplace;
    int stringLen;
  }editStatStruct;

  typedef struct node{
    word match;
    int matchrank;
    struct node *next;   
  }Node;

  typedef struct{
    uint32 start;
    uint32 end;
    uint32 fileSize;
    FILE *fp;
  }navigator;

  typedef struct{
    FILE *srcfp;
    int nPartitions;
    navigator *navList;
  }navigatorList;

#endif
