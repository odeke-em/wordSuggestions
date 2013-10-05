#ifndef _CONSTANTS_H
#define _CONSTANTS_H
  #define DICTIONARY_PATH "../resources/wordlist.txt"
  #define THRESHOLD_LEN 2 //Arbitrary value here, minimum length a word should
			  //have for consideration as a subject for suggestions

  #define THRESHOLD_PERCENT_RANK 70 //To be auto-calibrated in the future

  #define EXIT_CHAR '-'
  #define BUF_SIZ 30
  #define MAX_PATH 128

  #define INIT_HASH_VALUE 0
  #define UNDEF_HASH_VALUE -1
  #define INIT_N_HASH_ELEMS 20

  #define AUTOCORRECTION_USAGE \
    "Usage: \033[32m./autoCorrect <srcFile> <learntWordsPath>\
    [correctedTxtPath]\033[00m"
  #define MAX_CHARS_PER_LINE 100
#endif
