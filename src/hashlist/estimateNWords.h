#ifndef _ESTIMATE_N_WORDS_H
#define _ESTIMATE_N_WORDS_H
  #include <stdio.h>

  #define AVERAGE_WORD_LEN 8 // Arbitrary estimate
  typedef long long int LLInt;
  LLInt fileSize(FILE *ifp);
  LLInt estimatedWordCount(FILE *ifp, LLInt averageWLen);
#endif
