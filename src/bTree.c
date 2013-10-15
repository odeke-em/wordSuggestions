/*
  Author: Emmanuel Odeke <odeke@ualberta.ca>
  Balanced tree:
    Theory for balanced tree mostly learnt from: www.geeksforgeeks.org
*/

#include <string.h>
#include <stdlib.h>

#include "../include/bTree.h"
#include "../include/wordLib.h"

uint32 max(const int a, const int b) {
  return a > b ? a : b;
}

uint32 height(Tree *n) {
  if (n == NULL) return 0;

  return n->height;
}

int printTElem(FILE *outFile, const TElem e, DataType dT) {
  FILE *dest = stdout;
  if (outFile != NULL) {
    dest = outFile;
  }

  switch(dT) {
    case WORD: {
      fprintf(dest, "%s\n", e);
      break;
    }
  }
}

Tree *allocTree() {
  return (Tree *)malloc(sizeof(Tree));
} 

void initTree(Tree *t) {
  if (t == NULL) return;

  t->count = 0;
  t->height = 1;

  t->left  = NULL;
  t->right = NULL;
  t->dataType = WORD;
  t->key = NULL_ELEM;
  t->serialFile = NULL;
  t->printFunc = printTElem;
}

Tree *rightRotate(Tree *root) {
  Tree *temp = root->left;
  Tree *lRC = temp->right;

  //Rotating
  temp->right = root;
  root->left  = lRC;

  //Updating heights
  root->height = max(height(root->left), height(root->right))+1;
  temp->height = max(height(temp->left), height(temp->right))+1;

  return temp;
}

Tree *leftRotate(Tree *root) {
  Tree *temp = root->right;
  Tree *rLC = temp->left;

  //Performing the rotation
  temp->left = root;
  root->right = rLC;

  //Updating the heights
  root->height = max(height(root->left), height(root->right))+1;
  temp->height = max(height(temp->left), height(temp->right))+1;

  return temp;
}

//Balance factor of tree
int getBalance(Tree *n) {
  if (n == NULL) return 0;

  return height(n->left) - height(n->right);
}

Tree *insert(Tree *root, const TElem key, const ElemFuncStruct ElemFuncSt) {
  if (root == NULL) { 
    root = allocTree(); 
    initTree(root);
    root->key = ElemFuncSt.elemCopy(key);
    return root;
  }

  if (ElemFuncSt.comparator(key, root->key) == LT) {
    root->left = insert(root->left, key, ElemFuncSt);
  }else {
    root->right = insert(root->right, key, ElemFuncSt);
  }

  //Updating the root's height
  root->height = max(height(root->left), height(root->right))+1;

  //Getting the root's balance factor to check whether it became unbalanced
  int balance = getBalance(root);

  //If the root becomes unbalanced, handle the 4 consequential cases

  //Left Left case
  if (balance > 1 && ElemFuncSt.comparator(key, root->left->key) == LT) {
    return rightRotate(root);
  }

  //Right Right case
  if (balance < -1 && ElemFuncSt.comparator(key, root->right->key) == GT) {
    return leftRotate(root);
  }

  //Left Right case
  if (balance > 1 && ElemFuncSt.comparator(key, root->left->key) == GT) {
    root->left = leftRotate(root->left);
    return rightRotate(root);
  }

  //Right Left case
  if (balance < -1 && ElemFuncSt.comparator(key, root->right->key) == LT) {
    root->right = rightRotate(root->right);
    return leftRotate(root);
  }

  return root;
}

//Depth first order
void dfsOrder(Tree *root, FILE *serialFile){
  if (root != NULL) {
    dfsOrder(root->left, serialFile);
    root->printFunc(serialFile, root->key, root->dataType);
    dfsOrder(root->right, serialFile);
    fflush(root->serialFile);
  }
}

//Breadth first order
void bfsOrder(const Tree *root, const Tree *parent, FILE *serialFile) {
  if (root != NULL) {
    if (parent == NULL) {
      root->printFunc(serialFile, root->key, root->dataType);
    }

    if (root->left != NULL){
       root->printFunc(serialFile, root->left->key, root->dataType);
    }

    if (root->right!= NULL){
       root->printFunc(serialFile, root->right->key, root->dataType);
    }

    bfsOrder(root->left, root, serialFile);
    bfsOrder(root->right, root, serialFile);
  }
}

void preOrder(Tree *root, FILE *serialFile) {
  if (root != NULL) {
    root->printFunc(serialFile, root->key, root->dataType);
    preOrder(root->left, serialFile);
    preOrder(root->right, serialFile);
  }
}

Tree *addTElem(
  Tree *tree, const TElem newTElem, const ElemFuncStruct ElemFuncSt
) {
  if (newTElem == NULL_ELEM) return tree;
  if (tree == NULL) { //New element has arrived
  #ifdef DEBUG
    tree->printFunc(tree->serialFile, newTElem, tree->dataType);
  #endif
    tree = allocTree(); 
    initTree(tree);
    tree->key = ElemFuncSt.elemCopy(newTElem);
  }else {
    ElemComparison comp = ElemFuncSt.comparator(tree->key, newTElem);

    if (comp == EQ) { 
      ++tree->count;
    }else if (comp == LT) { 
      tree->right = addTElem(tree->right, newTElem, ElemFuncSt);
    }else {
      tree->left = addTElem(tree->left, newTElem, ElemFuncSt);
    }
  }


  return tree;
}

void freeTree(Tree *tree, const ElemFuncStruct ElemFuncSt) {
  if (tree == NULL) return;

  ElemFuncSt.freeTElem(tree->key);
  if (tree->left != NULL) {
    Tree *curTElem = tree->left;
    freeTree(curTElem->left, ElemFuncSt);
    freeTree(curTElem->right, ElemFuncSt);
    free(curTElem);
  }

  if (tree->right != NULL) {
    Tree *curTElem = tree->right;
    freeTree(curTElem->left, ElemFuncSt);
    freeTree(curTElem->right, ElemFuncSt);
    free(curTElem);
  }

  free(tree);
}

TElem intCopy(const TElem e) {
  return e;
}

void freeInt(TElem e) {
  return;
}

ElemComparison genericComp(const TElem a, const TElem b, DataType dT) {
  ElemComparison compResult = GT;
  switch(dT) {
    case INT: {
      if (a != b) {
	compResult = a < b ? LT : GT;
      }else compResult = EQ;
      break;
    }
    case FLOAT: {
      if (a != b) {
	compResult = a < b ? LT : GT;
      }else compResult = EQ;
      break;
    }
    case WORD: {
      compResult = strcmp(a, b);
      break;
    }

    default: {
      break;
    }
  }

  return compResult;
}

ElemComparison intComp(const TElem a, const TElem b) {
  return  genericComp(a, b, INT);
}

ElemComparison wordComp(const TElem a, const TElem b) {
  return  genericComp(a, b, WORD);
}

TElem treeSearch(Tree *tree, const TElem queryKey, const ElemFuncStruct eFST) {
  if (tree == NULL) return NULL_ELEM;

  ElemComparison comp = eFST.comparator(tree->key, queryKey);
  if (comp == EQ) return tree->key;

  else if (comp == GT)
    return treeSearch(tree->left, queryKey, eFST);

  else
    return treeSearch(tree->right, queryKey, eFST);
}

int serializeTree(Tree *tree, FILE *outFile, TraverseMode travMode){
  if (tree == NULL) return -1;

  switch(travMode){
    case DFS_TRAV: {
      dfsOrder(tree, outFile);
      break;
    }
  
    case BFS_TRAV: {
      bfsOrder(tree, NULL, outFile);
      break;
    }

    case PREORDER_TRAV: {
      preOrder(tree, outFile);
      break;
    }

    default: {
      dfsOrder(tree, outFile);
      break;
    }
  } 

  fflush(outFile);

  return 1;
}

Tree *deserializeTree(FILE *src) {
  if (src == NULL) return NULL;

  Tree *deserialTree = NULL;
  
  return deserialTree;
}

#ifdef SAMPLE_RUN 
int main() {
  ElemFuncStruct ElemFuncSt; 
  ElemFuncSt.freeTElem = freeWord;
  ElemFuncSt.comparator = wordComp;
  ElemFuncSt.elemCopy = strdup; //intCopy;
  //setTElem(ElemFuncSt, freeTElem, freeWord);

  word w[] = {"odeke", "emmanuel", "whom", "WhithHwom"};

  int wLen = sizeof(w)/sizeof(w[0]);
  int i=0;
  Tree *tree = NULL;

  for range(i, wLen) {
    tree = insert(tree, w[i], ElemFuncSt);
  }

  tree->dataType = WORD;

  preOrder(tree, stdout);
  printf("\033[33m\n");
  printf("\n");
  dfsOrder(tree, stdout);
  printf("\n");
  printf("\n\033[31m");
  bfsOrder(tree, NULL, stdout);
  printf("\n");
  printf("\n\033[32m");
  bfsOrder(tree, NULL, stdout);
  printf("\n");
  printf("\n\033[00m");
  printf("\n");

  FILE *outF = fopen("srlzTest.txt", "w");
#ifdef DEBUG
  printf("outF %p\n", outF);
#endif

  serializeTree(tree, outF, DFS_TRAV);
  fclose(outF);

  TElem found = treeSearch(tree, "whom", ElemFuncSt);

#ifdef DEBUG
  printf("outF %p\n", outF);
#endif

  printf("Found %s\n", found);

  freeTree(tree, ElemFuncSt);
  return 0;
}
#endif
