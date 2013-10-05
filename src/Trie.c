#include <stdlib.h>
#include <malloc.h>

#define MAX_EDGES 26

#include "../include/customTypes.h"
#include "../include/wordLib.h"

typedef struct trie {
  int words;
  int prefixes;
  struct trie *edges[MAX_EDGES];
} Trie;

Trie *initialize(Trie *node) {
  if (node == NULL)
    node = (Trie *)malloc(sizeof(Trie));

  node->words = 0;
  node->prefixes = 0;

  int i;
  for (i=0; i<MAX_EDGES; ++i)
    node->edges[i] = NULL;

  return node;
}

Trie *addWord(Trie *ver, word str) {
  if (str == NULL) return ver;

  printf("%c", *str);
  if (*str == '\0'){
    ++(ver->words);
  }else{
  #ifdef DEBUG
    printf("prefix %s\n", str);
  #endif
    ++(ver->prefixes);

    int index = *str-'a';
    if (ver->edges[index] == NULL){
      ver->edges[index] = initialize(ver->edges[index]);
    }

    ver->edges[index] = addWord(ver->edges[index], ++str);
  }

  return ver;
}

int countWords(Trie *ver, word str) {
  if (*str == '\0') return ver->words;

  else{
    int k = *str-'a';
    ++str;
    if (ver->edges[k] == NULL) return 0;

    return countWords(ver->edges[k], str);
  }
}

int countPrefix(Trie *ver, word str) {
  if (*str == '\0') return ver->prefixes;

  else{
    int k = *str-'a';
    ++str;
    if (ver->edges[k] == NULL) return 0;

    return countPrefix(ver->edges[k], str);
  }
}

void printTrie(Trie *ver) {
  if (ver == NULL) return;
}

#ifdef SAMPLE_RUN
int main() {
  Trie *start = NULL;
  start = initialize(start);

  FILE *fp = fopen(__FILE__, "r");

  word wIn = NULL;
  while (! feof(fp)) {
    wIn = getWord(fp, isalpha);
    if (printWord(wIn)) printWord("\n");
    start = addWord(start, wIn);
    freeWord(wIn);
  }

  if (wIn != NULL) {
    printWord(wIn);
    printWord("WIN is not NULL");
    printf("GOTCHA");
    start = addWord(start, wIn);
  }
  
  int ch=1;
  while (ch){
    printf("\n 1. Insert a word ");
    printf("\n 2. Count words ");
    printf("\n 3. Count prefixes ");
    printf("\n 0. Exit\n");
    printf("\n\033[33mEnter your choice: ");
    scanf("%d", &ch);
    char input[1000];
    switch(ch){
      case 1:
	printf("\n\033[31mEnter a word to insert: ");
	scanf("%s\033[00m", input);
	start = addWord(start, input);
	break;
      case 2:
	printf("\nEnter a word whose count you want: ");
	scanf("%s", input);
	printf("\nCount: %d\n", countWords(start, input));
	break;
      case 3:
	printf("\nEnter prefix whose count you want: ");
	scanf("%s", input);
	printf("\nPrefix count %d\n", countPrefix(start, input));
	break;
      case 0:
	printf("\nExiting...\n");
	ch = 0;
	break;
      default:
	printf("Going to case 2\n");
    }
  }

  if (start != NULL)
    free(start);

  return 0;
}
#endif
