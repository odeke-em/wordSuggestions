#include <stdio.h>
#include <stdlib.h>

#include "trie/Trie.h"
#include "hashlist/errors.h"
#include "hashlist/hashList.h"
#include "hashlist/loadWords.h"

#define USAGE_INFO __FILE__"\033[94m :: <dictionary> [...]\n\033[00m"

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, USAGE_INFO);
  } else {
    HashList *dict = loadWordsInFile("./resources/wordlist.txt");
    Trie *foundWordsTrie = createTrie(0); // Structure for 

    destroyHashList(dict);
    destroyTrie(memoizeTrie);
  }
  return 0;
}
