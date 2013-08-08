#ifndef _UTILITY_FUNCS_H
#define _UTILITY_FUNCS_H
  #include <stdio.h>
  #include <stdlib.h>
  #include <assert.h>
  #include <ctype.h>
  #include <string.h>
  #include <sys/stat.h>

  #include "customTypes.h"

  #define BUF_SIZ 30

  int notSpace(int c){
    return (! isspace(c));
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

  int getLine(word s, int max){
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
    word buf = (word)malloc(sizeof(char)*(BUF_SIZ));
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

  void toLower(word s){
    int len = strlen(s)/sizeof(char);
    int i=0;

    char c;
    while ((i<len) && (c = s[i])){
      if (isalpha(c)) s[i] = tolower(c); 
      ++i;
    }
  }

  Bool freeWord(word w){
    if (w == NULL) return False;
    free(w);
    return True;
  }

  int printWord(word w){
    return printf("%s", w);
  }

  Bool isValidFile(const word path, long *fileSize){
    //Input: A path 
    //Returns: True iff a path exists and is not a directory else False
    struct stat statInfo;
    if (stat(path, &statInfo) != 0) //Path doesn't exist or is null
      return False;

    if ((statInfo.st_mode <= 0))
      return False;

    if (! S_ISDIR(statInfo.st_mode)){
      *fileSize = statInfo.st_size;
      return True;
    }

    return False;
  }

  wordArrayStruct *wordArrStructAlloc(const int n){
    if (n <= 0) return NULL;
    wordArrayStruct *wArrSt = (wordArrayStruct *)malloc(sizeof(wordArrayStruct));
    assert(wArrSt != NULL);

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

  wordArrayStruct *wordsInFile(FILE *fp){
    int arraySize = 100;
    wordArrayStruct *wArrSt = wordArrStructAlloc(arraySize);
    if (fp == NULL) return NULL;
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
#endif
