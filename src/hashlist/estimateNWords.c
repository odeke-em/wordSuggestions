// Author: Emmanuel Odeke <odeke@ualberta.ca>
#include <stdio.h>
#include <assert.h>

#include "estimateNWords.h"

LLInt fileSize(FILE *ifp) {
  LLInt byteCount = 0;
  if (ifp != NULL) {
    long origHeadLocation = ftell(ifp);
    fseek(ifp, 0L, SEEK_END);
    byteCount = ftell(ifp);

    // Revert head back to where it was
    fseek(ifp, origHeadLocation, SEEK_SET);
  }

  return byteCount;
}

LLInt estimatedWordCount(FILE *ifp, LLInt averageWLen) {
  LLInt fSize = fileSize(ifp);

  if (! averageWLen) {
    averageWLen = AVERAGE_WORD_LEN;
  }

  return fSize && averageWLen ? fSize/averageWLen : 0;
}

#ifdef REV_WORD_ESTIMATOR
int main(int argc, char *argv[]) {
  FILE *ifp = NULL;
  if (argc < 2) {
    ifp =  fopen(__FILE__, "r");
  } else {
    ifp = fopen(argv[1], "r");
  }

  if (ifp != NULL) {
    long testOffSet = 100;

    fseek(ifp, testOffSet, SEEK_SET);
    long countBeforeCall = ftell(ifp);

    printf("fSize: %lld tmpSeek: %ld\n", fileSize(ifp), countBeforeCall);

    // Making sure that the head was reverted
    assert(countBeforeCall == ftell(ifp));

    LLInt wordCount = estimatedWordCount(ifp, 0);
    printf("Estimated wordCount: %lld\n", wordCount);
    fclose(ifp);
  }

  assert(estimatedWordCount(ifp, AVERAGE_WORD_LEN) == 0);

  return 0;
}
#endif
