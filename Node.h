/*Author: Emmanuel Odeke <odeke@ualberta.ca>*/
#ifndef _NODE_H
#define _NODE_H
  #include <string.h>
  #include <assert.h>
  #include <stdlib.h>
  typedef enum{
    False=0,
    True=1
  } Bool;

  typedef struct node{
    char *match;
    int matchrank;
    struct node *next;   
  }Node;

  int sortFunc(const void *, const void *);
  int rankComparison(const void *, const void *);
  int wordInNode(Node *tree, const char *word);
  
  char *wordCopy(const char *orig){
    if (orig == NULL) return NULL;

    char *copy = strdup(orig);
    assert(copy != NULL);

    return copy;
  }

  Node *nodeAlloc(void){ return (Node*)malloc(sizeof(Node));}
  void nodeFree(Node *tree){
    Node *tmp;
    while(tree != NULL){
      tmp = tree->next;
      if (tree->match != NULL)
	free(tree->match);

      free(tree);
      tree = NULL;
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

  Node *addWord(Node *tree, const char *word, int rankMatch){
    if ( word == NULL) return tree;

    if (tree == NULL){
      tree = nodeAlloc();
      tree->match = wordCopy(word);
      if (tree->match == NULL) printf("found null word\n");
      assert(tree);

      tree->matchrank = rankMatch;
      tree->next  = NULL;
    }
    else if ((tree->match != NULL) && (strcmp(tree->match,word) != 0)){
      tree->next = addWord(tree->next,word,rankMatch);
    }
  
    return tree; 
  }

  Bool serializeNode(Node *tree, FILE *outfp){
    Node *tmp;
    Bool wroteToFileBool = False;
    if (tree != NULL){
      for (tmp = tree; tmp != NULL; tmp=tmp->next){
	fprintf(outfp, "%s\n", tmp->match);
      }
      wroteToFileBool = True;
    }

    return wroteToFileBool;
  }

  int wordInNode(Node *tree, const char *word){
    /*
      Returns: 
	   0  if tree or word are NULL
	   1  if word was found
	  -1 if word was not found
    */
    if ((word == NULL) || (tree == NULL)) return 0;
    
    struct node *tmp;
    for (tmp=tree; tmp != NULL; tmp=tmp->next){
      //Let's check if the length, first and last letters are the same
      const char *tmpW = tmp->match;
      int tmpLen = strlen(tmpW), queryLen = strlen(word);
      int lastIdx = tmpLen-1;

      if (! ((tmpW[0] == word[0]) && (tmpW[lastIdx] == word[lastIdx]))) 
	continue;

      if (tmpLen != queryLen) continue;

      if (strcmp(word, tmpW) == 0) return 1;
    }
    return -1;
  }
#endif 
