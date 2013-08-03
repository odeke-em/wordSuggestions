/*Author: Emmanuel Odeke <odeke@ualberta.ca>*/
#ifndef _NODE_H
#define _NODE_H
  #include <string.h>
  #include <assert.h>
  #include <stdlib.h>

  #include "customTypes.h"

  int wordInNode(Node *, const word);
  Bool sameWord(const word, const word);
  int sortFunc(const void *, const void *);
  int rankComparison(const void *, const void *);
  
  word wordCopy(const word orig){
    if (orig == NULL) return NULL;

    word copy = strdup(orig);
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

  Node *addWord(Node *tree, const word query, int rankMatch){
    if (query == NULL) return tree;

    if (tree == NULL){
      tree = nodeAlloc();
      tree->match = wordCopy(query);
      if (tree->match == NULL) printf("found null word\n");
      assert(tree);

      tree->matchrank = rankMatch;
      tree->next  = NULL;
    }
    else if (sameWord(tree->match, query) == False){
	tree->next = addWord(tree->next, query, rankMatch);
    }
  
    return tree; 
  }

  Bool sameWord(const word w1, const word w2){
    if (w1 == NULL || w2 == NULL) return Invalid;
    int w1Len = strlen(w1), queryLen = strlen(w2);

    if (w1Len != queryLen) return False;

    int lastIdx = w1Len-1;
    Bool extremeLetterMatch = (w1[0] == w2[0]) && (w1[lastIdx] == w2[lastIdx]);
    if (extremeLetterMatch == False) return False;

    return (strcmp(w1, w2) == 0);
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
#endif 
