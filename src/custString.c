#include <stdlib.h>
#include <string.h>
#include "../include/custString.h"

void initString(String *s){
  if (s == NULL){
    raiseWarning(
      "Expecting non-NULL string to be passed in for initialization"
    );
    return;
  }

  s->startIndex = 0;
  s->endIndex = 0;
  s->len = 0;
  s->content = NULL;
}

void freeString(String *s){
  if (s == NULL) return;
  freeWord(s->content);
  free(s);
}

String *allocString(void){
  return (String *)malloc(sizeof(String));
}

String *toString(const word w){
  String *newString = allocString();
  assert(newString);

  setStringContent(newString, w);
  return newString;
}

void setStringContent(String *st, const word w){
  if (st == NULL){
    raiseWarning("The string container is NULL");
    return;
  }

  if (w == NULL){
    raiseWarning("The word to be added is NULL");
    return;
  }

  freeWord(st->content);
  st->content = strdup(w);
  int wordLen = strlen(w)/1;
  st->startIndex = 0;
  st->endIndex = wordLen-1;
  st->len = wordLen;
}

Bool stringIntegrity(const String *s){
  if (s == NULL) return True;

  if (s->len < 0) return False;
  if (s->len != (strlen(s->content)/sizeof(char))) return False;
  if ((s->startIndex < 0) || (s->endIndex < s->startIndex)) return False;
  if (s->len <= s->endIndex) return False;

  return True;
}

Bool isSubString(const String *query, const String *base){
  if ((query == NULL) || (base == NULL)) return Invalid;

  if (stringIntegrity(query) != True){
    throwException(
      CorruptedDataException, "Query string [argument 1] is corrupted"
    );
  }

  if (stringIntegrity(base) != True){
    throwException(
      CorruptedDataException, "Base string [argument 2] is corrupted"
    );
  }

  if ((query->content == NULL) || (base == NULL)) return False;

  int i=query->startIndex, iExtreme=query->endIndex; 
  int j=base->startIndex, jExtreme=base->endIndex; 

  int iSubIndexLen = iExtreme-i;
  int jSubIndexLen = jExtreme-j;

  if ((iSubIndexLen < 0) || (iSubIndexLen != jSubIndexLen)) return False;
  #ifdef DEBUG
    printf("i %d j %d iExtreme %d jExtreme %d\n", i, j, iExtreme, jExtreme);
  #endif
  int queryMidLen = (i+iExtreme)/2;
  int iEnd = queryMidLen;
  while (i < queryMidLen){
    if (charAt(query, i) != charAt(base, i)) return False;

    if (iEnd >= iExtreme) continue;
    if (charAt(query, iEnd) != charAt(base, iEnd)) return False;

    ++i, ++iEnd;
  }

  return True;
}

char charAt(const String *s, const int i){
  if (s == NULL){
    throwException(
      NullPointerException, "NULL string passed in for comparison"
    ); 
  }
  if (i > s->len){
    throwException(IndexError, "Index i is out of string length's range");
  }

  return (s->content)[i];
}

Bool isEqual(const String *s1, const String *s2){
  return False;
}

String *slice(const String *s, const int start, const int end){
  if (stringIntegrity(s) != True){
    throwException(
      CorruptedDataException, "String source is corrupted"
    );
  }

  if (start >= end){
    throwException(
      IndexError, "start index should be less than the end"
    );
  }

  String *newSlice = allocString();
  initString(newSlice);

  int range = end-start;
  newSlice->content = newWord(range);

  int i, midRange = range/2;
  for (i=0; i<=midRange; ++i){
    (newSlice->content)[i] = (s->content)[i+start];
    volatile int iExtreme = i+midRange;
    if (iExtreme >= end) continue;
    (newSlice->content)[iExtreme] = (s->content)[iExtreme+start];
  }
 
  return newSlice; 
}

#ifdef SAMPLE_RUN
int main(){
  String *st1 = allocString();
  String *st2 = allocString();

  initString(st1);
  initString(st2);

  setStringContent(st1, "Advertising\n");
  setStringContent(st2, "Advertisinging\n");

  String *sl = slice(st1, 0, 7);

  st2->endIndex = st1->endIndex;

  printf("%c at %d\n", charAt(st2, st2->endIndex), st2->endIndex);
  printf("%c at %d\n", charAt(st1, st1->endIndex), st1->endIndex);

  Bool isSub = isSubString(st1, st2);
  printf("IsSubS %d\n", isSub);

  printWord(st1->content);
  printWord(st2->content);
  printWord(sl->content);

  freeString(st1);
  freeString(st2);
  freeString(sl);

  return 0;
}
#endif
