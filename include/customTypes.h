#ifndef _CUSTOM_TYPES_H
#define _CUSTOM_TYPES_H
  #include <stdio.h>
  typedef char *word;

  typedef long int hash;
  typedef unsigned int uint32;
  typedef long long int LLInt;

  typedef enum{
    Invalid=-1,
    False=0,
    True=1
  } Bool;

  //Singly linked list to store indices of found letters
  typedef struct indexNode{
    int index;
    int count;
    char value;
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
    uint32 len;
    int matchrank;
    struct node *next;   
  }Node;

  //Struct used to navigate a file with the purpose of 
  //partitioning it thus enabling multi-threading
  typedef struct{
   char wi, wj;
   int i, j;
   Bool *foundCh;
  }transientStruct;

  typedef struct{
    word *wordArray;
    int n;
  }wordArrayStruct;

  typedef struct{
    uint32 start;
    uint32 end;
    uint32 fileSize;
    FILE *srcfp;
    wordArrayStruct *dictWArrayStruct;
    char *toPath;
  }navigator;

  typedef struct{
    FILE *srcfp;
    int nPartitions;
    navigator *navList;
  }navigatorList;

  typedef struct{
    word wordValue;
    hash hashValue;
  }hashElem;

  typedef struct{
    hashElem *hEArray;
    uint32 arrSize;
    uint32 currentIndex;
  }hashList;

  typedef struct{
    uint32 start, end;
  }sliceStruct;

  typedef struct{
    int startIndex, endIndex, len; 
    word content;
  }String;

  //Comparator results
  typedef enum{
    LT=-1, EQ=0, GT=1
  }ElemComparison;

  typedef struct{
    Bool lenMatch_bool;
    Bool firstLetterMatch_bool;
  } wordMatchCriteria; //Struct to pass a pair of Bools 
	      //eg, LEN_MATCH_BOOL and FIRST_LETTER_MATCH
#endif
