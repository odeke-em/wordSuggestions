/*Author: Emmanuel Odeke <odeke@ualberta.ca>*/
#ifndef _WORD_FUNCS
#define _WORD_FUNCS

  #include <stdio.h>
  #include <unistd.h>
  #include <stdlib.h>
  
  typedef char *word;
  typedef int boolean;
  typedef int number;
  typedef word sentence;

  #define STRCMPEQUAL(cond)  (cond ==  0)
  #define STRCMPLESS(cond)  (cond < 0)
  #define STRCMPGREATER(cond)  (cond > 0)

  #define SPACE   ' '
  #define FOUND 1
  #define STR_NOT_FOUND -1
  #define TRUE   1
  #define FALSE  0

  #define NEWLINE '\n'

  #define  requestBufInSufficient\
      fprintf(stderr, "The buffer has less space than requested\n");\

  #define raiseError(args) {\
    fprintf(stderr, "Traceback most recent call at line: %d ", __LINE__);\
    fprintf(stderr, "of file: %s\nExpression: %s is invalid\n", __FILE__, #args);\
    exit(0);}\


  #define assert(validExpression) \
    if (! validExpression)\
      raiseError(validExpression);\

  struct node {
    char *name;
    int count;
    struct node *left;
    struct node *right;
  };

  void *nodeAlloc(void){
    return (struct node *)malloc(sizeof(struct node));
  }

  void treePrint(struct node *tree){
    if (tree != NULL){
      treePrint(tree->left);

      fprintf(stderr, "Word: %s Count: %d\n",
          tree->name, tree->count);
      treePrint(tree->right);
    }
  }

  word strAlloc(word orig){
    word copy = (word)malloc(sizeof(char)* strlen(orig));
    strcpy(copy, orig);
    return copy;
  }

  struct node *addTree(struct node *parent, word searchWord){
    if (parent == NULL){
      parent        = nodeAlloc();
      parent->name  = strAlloc(searchWord);
      parent->count  = 1;
      parent->left  = NULL;
      parent->right  = NULL;
  } else {
    int comparison = strcmp(parent->name, searchWord);
    if  (STRCMPEQUAL(comparison)) 
        parent->count++;

    else if (STRCMPGREATER(comparison ))
        parent->right = addTree(parent->right, searchWord);

    else 
        parent->left  = addTree(parent->left,  searchWord);

    }
    return parent;
  }
  
  number getWord(FILE *stream, word wbuffer, number nChars){
    char chBuf;
    number  index = 0;

    while (
      ((chBuf = getc(stream)) != NEWLINE) && 
      (chBuf != EOF) &&
      (chBuf != ' ') &&
      (! isspace(chBuf)) &&
      (index < nChars)){
        wbuffer[index++] = chBuf;
    }
    wbuffer[ index] = '\0';  
    return ((chBuf == EOF) ? EOF : index);
  }

  boolean wordinTree(struct node *tree, word searchWord){
    if (tree == NULL)
      return STR_NOT_FOUND;

    int comparison = strcmp(tree->name, searchWord);
    if (STRCMPEQUAL(comparison))
      return FOUND;
    else if(STRCMPLESS(comparison))
      return wordinTree(tree->left, searchWord);
    else
      return wordinTree(tree->right, searchWord);
  }

  FILE *fopener(char *fname, char *perms){
    FILE *ofp = fopen(fname, perms);
    boolean isValidFile = (ofp != NULL);
    assert(isValidFile);

    return ofp;
  }
#endif
