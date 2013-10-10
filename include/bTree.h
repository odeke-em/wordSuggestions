#ifndef _BST_H
#define _BST_H
  #include <stdio.h>
  #include "customTypes.h"

  //Helper macros
  #define range(i, n) (; i<n; ++i)
  #define foreach(i, n) for (; i != n; ++i)
  #define setTElem(obj, attr, value) {\
    obj.attr = value;\
  }

  typedef word TElem;

  typedef enum{
    BFS_TRAV, DFS_TRAV, PREORDER_TRAV
  } TraverseMode;

  typedef enum{
    INT, FLOAT, WORD, NODE
  }DataType;

  #ifndef NULL_ELEM
    #define NULL_ELEM NULL
  #endif

  typedef struct tnode{
    TElem key;  
    uint32 count, height;  
    int (*printFunc)(FILE *, const TElem, DataType);
    struct tnode *left;
    struct tnode *right;
    DataType dataType;
    FILE *serialFile; //Outfile to which serialization will be made
  }Tree;

  //Structure containing utility functions for operations 
  //eg copying, comparing elements
  typedef struct{
    ElemComparison (*comparator)(const TElem, const TElem);
    TElem (*elemCopy)(const TElem);
    void (*freeTElem)(TElem);
  }ElemFuncStruct;

  Tree *allocTree(); 

  //Given a tree:
  // if the tree is NULL: do nothing
  // Else: 
  //      Set the height to 1, key count to 0, 
  //        serialFile(for serialization) to NULL, dataType to WORD(by default)
  void initTree(Tree *);

  void freeTree(Tree *, const ElemFuncStruct);

  Tree *addTElem(Tree *, const TElem, const ElemFuncStruct);

  ElemComparison wordComp(const TElem, const TElem);

  //Tree traversal methods
  void preOrder(Tree *, FILE *);
  void dfsOrder(Tree *, FILE *);
  void bfsOrder(const Tree *root, const Tree *parent, FILE *) ;

  
  int serializeTree(Tree *, FILE *, TraverseMode);

  Tree *deserializeTree(FILE *);
#endif
