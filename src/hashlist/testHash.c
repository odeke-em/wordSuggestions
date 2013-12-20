// Author: Emmanuel Odeke <odeke@ualberta.ca>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "errors.h"
#include "hashList.h"
#include "loadWords.h"

#define tolower(x) (x | 'a' - 'A')
#define WORD_INCREMENT_LEN 10

int main() {
  HashList *dict = loadWordsInFile("../../resources/wordlist.txt");
  printf("Dict: %p\n", dict);
  FILE *ifp = fopen(__FILE__, "r");
  while (! feof(ifp)) {
    char *inW = getWord(ifp);
    if (inW == NULL) continue;

    Element *match = getCloseMatches(inW, dict, 0.7);
    printf("For: %s\n", inW);
    if (inW != NULL) free(inW);
    while (match != NULL) {
      printf("\t%s\n", (char *)match->value);
      match = getNext(match);
    } 
  }

  destroyHashList(dict);
  fclose(ifp);
  return 0;
}
