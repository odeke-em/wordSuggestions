/*
  Author: Emmanuel Odeke <odeke@ualberta.ca>
*/
#ifndef _WORD_SEARCH_H
#define _WORD_SEARCH_H
  #include <stdio.h>
  #include <unistd.h>
  #include <stdlib.h>
  #include <string.h>

  #include "Node.h"
  #include "utilityFuncs.h"
  #include "wordTransition.h"
  #include "wordLib.h"
  #include "bTree.h"

  int wordSimilarity(const word, const word, const Bool);

  //Find words whose similarity to the query word is above the threshold 
  // match percentage. Add these similar words to the singly linked list:
  //argument 3 -- storageNode.
  Node *getSuggestions(
    const wordArrayStruct *, FILE *, Node *,
    const word, const wordMatchCriteria
  );

  Tree *getMatches(
    const wordArrayStruct *, FILE *, Tree *, const word,
    const wordMatchCriteria, const ElemFuncStruct
  );
#endif
