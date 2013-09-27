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

  #define DICTIONARY_PATH "../resources/wordlist.txt"
  #define THRESHOLD_LEN  2 //Arbitrary value here
  #define THRESHOLD_PERCENT_RANK 70 //To be auto-calibrated in the future

  int wordSimilarity(const word, const word, const Bool);

  Node *loadWord(
    const wordArrayStruct *, FILE *, Node *,
    const word, const wordMatchCriteria
  );

  Tree *getMatches(
    const wordArrayStruct, const Tree *, FILE *, 
    Tree *, const word, const Bool, const Bool
  );
#endif
