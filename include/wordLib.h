#ifndef _WORD_LIB_H
#define _WORD_LIB_H
  #include <stdio.h>
  #include <stdlib.h>
  #include <ctype.h>
  #include <string.h>

  #include "customTypes.h"
  #include "errors.h"

  #define BUF_SIZ 30
  //Copy the string of alphabetic characters first 
  //encountered by the file reader's latest position
  word getWord(FILE *);

  int printWord(const word);

  //Return n-characters worth of a word
  word newWord(const int);
  Bool freeWord(word);

  //Set all characters of a word to lower case
  void toLower(word s);

  //Return struct whose word array contains n-words
  wordArrayStruct *wordArrStructAlloc(const int);

  //Set the size of the word array to n, shrinking or expanding it
  //depending on it's size at the time the function is called
  wordArrayStruct *wordArrStructReSize(wordArrayStruct *, const int);
  void freeWordArrayStruct(wordArrayStruct *);
  int printWordArrayStruct(const wordArrayStruct *);

  //Loads all the words from a file into a word-array struct
  wordArrayStruct *wordsInFile(const word);

  int notSpace(const int);

  //Given an initialized word of n-characters, copy from stdin 
  //until a new line character or the nth-index
  int getLine(word, const int);
  int skipTillCondition(FILE *, int(*condFunc)(int));


  //Binary-search for a word-array struct
  int bSearch(const wordArrayStruct *wArrStruct, const word query);

  Bool sameWord(const word, const word);
#endif
