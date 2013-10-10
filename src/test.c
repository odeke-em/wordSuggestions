#include <stdio.h>
#include <stdlib.h>

#include "../include/bTree.h"
#include "../include/wordLib.h"
#include "../include/wordSearch.h"

int main() {
  wordArrayStruct *dictWArray = wordsInFile("../resources/wordlist.txt");
  FILE *ofp = fopen("odekeCorrected.txt","w");
  Tree *matches = NULL;
  ElemFuncStruct ElemFuncSt; 
  ElemFuncSt.freeTElem = freeWord;
  ElemFuncSt.comparator = wordComp;
  ElemFuncSt.elemCopy = strdup;
  wordMatchCriteria matchCriteria;
  matchCriteria.lenMatch_bool = matchCriteria.firstLetterMatch_bool = True;
  matches = getMatches(
      dictWArray, ofp, matches, "cypher", matchCriteria, ElemFuncSt
  );

  bfsOrder(matches, NULL, stdout);
  serializeTree(matches, ofp, BFS_TRAV);
  freeTree(matches, ElemFuncSt);
  fclose(ofp);
  return 0;
}
