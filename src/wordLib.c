#include "../include/wordLib.h"

int notSpace(const int c){
  return (! isspace(c));
}


int printWord(const word w){
  return printf("%s", w);
}

word newWord(const int n){
  if (n <= 0) return NULL;
  return (word)malloc(sizeof(char)*n);
}

int getLine(word s, const int max){
  int i=0;
  char c=EOF;
  while ((i < max) && ((c = getchar()) != EOF)){
    if ((c == ' ') || (c == '\n' && putchar(c))){
    s[i] = '\0';
    break;
    }
    s[i] = c;  
    i++;
  }
  return ((c == EOF) ? EOF : i);
}

word getWord(FILE *fp){
  //Copies only alphabetic characters. A non-alphabetic character signals 
  //the function to stop reading and return the already found content
  word buf = newWord(BUF_SIZ);
  char c='0';
  int i=0;

  if (buf == NULL){
    fprintf(stderr, "Run out of memory in func %s on line %d in file %s\n",
      __func__,__LINE__,__FILE__
    );
    exit(-1);
  }

  while (! feof(fp)){
    if (i>=BUF_SIZ) break;

    int nRead = fread(&c,1,sizeof(char), fp);
    if (nRead == -1){
    fprintf(
     stderr,"Failed to read a char from fp in function %s in file %s\n",
      __func__, __FILE__
    );
    exit(-2);
    }
 
    if (! isalpha(c)){
    buf[i] = '\0';
    break;
    }else{
    buf[i] = c;
    ++i;
    }
  }

  #ifdef DEBUG
    fprintf(stderr,"got_word %s\n",buf);
  #endif
  return buf;
}

void freeWordArrayStruct(wordArrayStruct *wASt){
  if (wASt == NULL) return;
  if (wASt->wordArray != NULL){
    int i;
    for (i=0; i<wASt->n; ++i){
	word tmp;
	tmp = (wASt->wordArray)[i];
	freeWord(tmp);
    }
    free(wASt->wordArray);
  }
}

int printWordArrayStruct(const wordArrayStruct *wASt){
  if ((wASt == NULL) || (wASt->wordArray == NULL)) return -1;
  int i, n = wASt->n;
  for (i=0; i<n; ++i){
    printWord((wASt->wordArray)[i]);
    printWord("\n");
  }
  return i;
}


wordArrayStruct *wordArrStructAlloc(const int n){
  if (n <= 0) return NULL;
  wordArrayStruct *wArrSt = \
	(wordArrayStruct *)malloc(sizeof(wordArrayStruct));
  assert(wArrSt);

  wArrSt->wordArray = (word *)malloc(sizeof(word)*n);
  wArrSt->n = n;

  return wArrSt;
}

wordArrayStruct *wordArrStructReSize(wordArrayStruct *wASt, const int newN){
  if ((newN > 0) && (wASt != NULL) && (wASt->wordArray != NULL)){
    if (newN != wASt->n){
	wASt->wordArray = (word *)realloc(wASt->wordArray, sizeof(word)*newN);
	wASt->n = newN;
    }
  }
  return wASt; 
}

wordArrayStruct *wordsInFile(const word filePath){
  FILE *fp = fopen(filePath, "r");
  if (fp == NULL) return NULL;
  int arraySize = 100;
  wordArrayStruct *wArrSt = wordArrStructAlloc(arraySize);
  int wordIndex = 0;
  while (!feof(fp)){
    if (wordIndex >= arraySize){
	arraySize *= 2;
	wArrSt = wordArrStructReSize(wArrSt, arraySize);
    }
    word theWord = getWord(fp);
    wArrSt->wordArray[wordIndex] = strdup(theWord);
    free(theWord);
    ++wordIndex;
  }
  wArrSt = wordArrStructReSize(wArrSt, wordIndex);

  return wArrSt;
}

int skipTillCondition(FILE *ifp, int(*condFunc)(int)){
  if (ifp == NULL) return EOF;
  
  char c;
  int nSkips=0;
  while (! feof(ifp)){
    c = getc(ifp);
    if (condFunc(c)){
	ungetc(c, ifp);
	break;
    }
    ++nSkips;
  }

  return nSkips;
}

Bool freeWord(word w){
  if (w == NULL) return False;
  free(w);
  return True;
}

void toLower(word s){
  int len = strlen(s)/sizeof(char), i=0;

  char c;
  while ((i<len) && (c = s[i])){
    if (isalpha(c)) s[i] |= 'a'-1;
    ++i;
  }
}

int bSearch(const wordArrayStruct *wArrStruct, const word query){
  if ((wArrStruct == NULL) || (wArrStruct->wordArray == NULL)) return -1;
  int start = 0, end = wArrStruct->n - 1;
  word *wArray = wArrStruct->wordArray;
  int mid;

  while (start <= end){
    if (strcmp(query, wArray[start]) == 0) return start;
    if (strcmp(query, wArray[end]) == 0) return end;
    mid = (start+end)/2;
    word midWord = wArray[mid];
    int midWordComparison = strcmp(query, midWord);
    if (midWordComparison == 0) return mid;
    else if (midWordComparison < 0){
      start += 1;
      end = mid-1;
    }else{
      start = mid+1;
      end -= 1;
    }
  }
    
  return -1;
}
