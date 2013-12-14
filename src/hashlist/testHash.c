// Author: Emmanuel Odeke <odeke@ualberta.ca>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "errors.h"
#include "hashList.h"

#define WORD_INCREMENT_LEN 10

char *getWord(FILE *ifp) {
  int wLen = WORD_INCREMENT_LEN;
  char *readChars = NULL;
  if (ifp  != NULL) {
    readChars = (char *)malloc(sizeof(char) * wLen);
    if (readChars == NULL) raiseError("Run out of memory");

    int i = 0;
    char c;
    while ((c = getc(ifp)) != EOF) {
      if (i >= wLen) {	
	wLen += WORD_INCREMENT_LEN;
	readChars = (char *)realloc(readChars, sizeof(char) * wLen);
      }

      if (isspace(c)) {
	break;
      } else if (isalpha(c)){
	readChars[i++] = c;
      }
    }

    if (i) { 
      readChars[i]= '\0';
      readChars = (char *)realloc(readChars, sizeof(char) * (i+1));
    }
  }

  return readChars;
}

int main() {
  char *w[] = {"odeke\0", "odeka\0", "oedke\0", "emmanuelodekeiciamigo\0"};
  int i = sizeof(w)/sizeof(w[0]);
  while (--i > -1) {
    hashValue h = pjwCharHash(w[i]);
    printf("%s %d\n", w[i], h);
  }

  HashList *hl = NULL;

  hl = initHashListWithSize(hl, 1000000);

  FILE *ifp = stdin;
  while (! feof(ifp)) {
    char *inW = getWord(ifp);
    insertElem(hl, inW, pjwCharHash(inW));
  }

  hashValue queryHash = pjwCharHash("odekee\0");
  Element **queryValue = get(hl, queryHash);

  if (queryValue != NULL) {
    Element *tg = *queryValue;
    while (tg != NULL) {
      printf("Found: %s\n", (char *)tg->value);
      tg = tg->next;
    }
  }

  destroyHashList(hl);
  return 0;
}
