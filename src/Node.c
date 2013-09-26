#include "../include/Node.h"
  
word wordCopy(const word orig) {
  if (orig == NULL) return NULL;

  word copy = strdup(orig);
  assert(copy);

  return copy;
}

Node *nodeAlloc(void) { 
  return (Node*)malloc(sizeof(Node));
}

void nodeFree(Node *tree){
  Node *tmp;
  while(tree != NULL){
    tmp = tree->next;
    if (tree->match != NULL)
      free(tree->match);

    free(tree);
    tree = tmp;
  }
  tree = NULL;
}

int nodePrint(FILE *fp, Node *tree){
  /*
    Input: a singly linked list 'tree'
    Output: printed attributes of the tree, 'match', matchrankage
    Returns: the number of non-NULL nodes in the tree
  */
  if (fp == NULL) fp =stdout;
  int nPrints = 0;
  Node *tmp;
  int maxPrintPerLine=4;
  if (tree != NULL){
  #ifdef INTERACTIVE
    fprintf(stderr," Suggestions: \033[32m");
  #endif

    fprintf(fp, "{ \t\n");
    for (tmp = tree; tmp != NULL; tmp=tmp->next){
      fprintf(fp, "%s:%d ",tmp->match,tmp->matchrank);

      ++nPrints;
      if (! nPrints%maxPrintPerLine) fprintf(fp, "\n");
    }

    fprintf(fp, "}\n");
  #ifdef INTERACTIVE
    fprintf(stderr,"\t\n\033[00m");
  #endif
  }

  return nPrints;
}

Node *addWord(Node *tree, const word query, int rankMatch){
  if (query == NULL) return tree;

  if (tree == NULL){
    tree = nodeAlloc();
    tree->match = wordCopy(query);
    if (tree->match == NULL) printf("found null word\n");
    assert(tree);

    tree->matchrank = rankMatch;
    tree->next  = NULL;
  }else if (sameWord(tree->match, query) == False){
    tree->next = addWord(tree->next, query, rankMatch);
  }
  
  return tree; 
}

Bool serializeNode(Node *tree, FILE *outfp){
  Node *tmp;
  int nWrites = 0;
  if (tree != NULL){
    for (tmp = tree; tmp != NULL; tmp=tmp->next){
      fprintf(outfp, "%s\n", tmp->match);
      ++nWrites;
    }
  }

  return nWrites ? True : False;
}

Bool wordInNode(Node *tree, const word query){
  /*
   Returns: 
    Invalid  if either tree or word is NULL
    True  if word was found
    False otherwise
  */
  if ((query == NULL) || (tree == NULL)) return Invalid;
    
  struct node *tmp;
  for (tmp=tree; tmp != NULL; tmp=tmp->next){
    Bool stringMatch = sameWord(query, tmp->match);
    if (stringMatch == True) return True;
    else if (stringMatch == Invalid) return Invalid;
  }
  return False;
}
