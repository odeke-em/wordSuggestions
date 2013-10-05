#include "../include/Node.h"
#include "../include/constants.h"
  
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
  if (tree != NULL) {

    Node *tmp;
    while(tree != NULL) {
      tmp = tree->next;
      if (tree->match != NULL)
	free(tree->match);

      free(tree);
      tree = tmp;
    }
    tree = NULL;
  }
}

LLInt nodePrint(FILE *fp, Node *tree){
  /*
    Input: a singly linked list 'tree'
    Output: printed attributes of the tree, 'match', matchrankage
    Returns: the number of non-NULL nodes in the tree
  */
  if (fp == NULL) fp =stdout;
  Node *tmp;
  int maxPrintPerLine=4;

  LLInt printCount = 0;
  if (tree != NULL){
  #ifdef INTERACTIVE
    fprintf(fp," Suggestions: \033[32m");
  #endif

    fprintf(fp, "{ \n\t");
    for (tmp = tree, printCount=0; tmp != NULL; tmp=tmp->next){
      printCount += tmp->len;
      if (printCount > MAX_CHARS_PER_LINE) {
	fprintf(fp, "\n\t");
	printCount = 0;
      }

      printCount += fprintf(fp, "%s:%d ",tmp->match,tmp->matchrank);

    }

    fprintf(fp, "\n} ");
  #ifdef INTERACTIVE
    fprintf(fp,"\t\n\033[00m");
  #endif
  }

  return printCount;
}

Node *addWord(Node *tree, const word query, const int queryLen, int rankMatch){
  if (query == NULL) return tree;

  if (tree == NULL){
    tree = nodeAlloc();
    tree->match = wordCopy(query);
    if (tree->match == NULL) printf("found null word\n");
    assert(tree);

    tree->matchrank = rankMatch;
    tree->len = queryLen;
    tree->next  = NULL;
  }else if (sameWord(tree->match, query) == False){
    tree->next = addWord(tree->next, query, queryLen, rankMatch);
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
