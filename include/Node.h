/*Author: Emmanuel Odeke <odeke@ualberta.ca>*/
#ifndef _NODE_H
#define _NODE_H
  #include <string.h>
  #include <stdlib.h>

  #include "errors.h"
  #include "customTypes.h"
  #include "wordLib.h"

  void nodeFree(Node *tree);
  int wordInNode(Node *, const word);
  int sortFunc(const void *, const void *);
  int rankComparison(const void *, const void *);

  int nodePrint(FILE *, Node *);
  Bool serializeNode(Node *, FILE *);
  Node *addWord(Node *, const word query, int);
#endif 
