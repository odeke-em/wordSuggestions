#ifndef _CONJUGATOR_H
#define _CONJUGATOR_H
  #include "errors.h"
  #include "customTypes.h"
  #include "utilityFuncs.h"

  /**Conjugations starting here**/
  #define NOUN
  #define TENSE
  #define ADVERB
  #define PRONOUN

  //Initializes a string, setting the start and endIndices to zero,
  //String content to NULL and string length to zero 
  void initString(String *);

  void freeString(String *);
  
  //Returns a String-worth of memory
  String *allocString(void);

  //Similar to indexing the String's contents -> contents[n]
  char charAt(const String *, const int ); 

  //Returns True if the given string's start is less than or equal to 
  //endIndex. Also the length of the content should be what it is 
  //purported to be
  Bool stringIntegrity(const String *);

  //Returns True if argument 1's contents with it's start and endIndices
  //match those also in the range of argument 2's contents 
  Bool isSubString(const String *, const String *);

  //Returns a string containing characters within range start and end 
  String *slice(const String *, const int, const int);

  //Set the string's content to the word argument
  void setStringContent(String *, const word);

  //word to string converter
  String *toString(const word);
#endif
