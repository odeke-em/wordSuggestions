/*Author: Emmanuel Odeke <odeke@ualberta.ca>*/
#ifndef _WORD_SEARCH_H
#define _WORD_SEARCH_H
  #include <ctype.h>
  #include <unistd.h>
  #include <stdlib.h>
  #include <string.h>
  #include <sys/stat.h>

  #include "Node.h"
  #include "wordTransition.h"

  #define BUF_SIZ 30
  #define THRESHOLD_RANK -2
  #define THRESHOLD_LEN  2
  
  typedef unsigned int uint32;

  char *getWord(FILE *);
  int wordSimilarity(const char *query, const char *src,Bool LEN_MATCH_BOOL);

  Bool isValidFile(char *path, long *fileSize){
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

  Node *loadWord(FILE *fp, FILE *correctedDest, Node *storageNode,const char *query,
    Bool LEN_MATCH_BOOL, Bool FIRST_LETTER_MATCH){
    /*
      Find words whose similarity to the query word is above the threshold 
      match percentage. Add these similar words to the singly linked list:
	  'storageNode'.
    */
    if ((fp == NULL) || (query == NULL) || (strlen(query) == 0))
      return storageNode;

    //Rewind file to the very beginning
    fseek(fp, 0, SEEK_SET);
    int alreadyInStorage = 0; 
    char *wordBuf = NULL; 
    Node *wordNode = NULL;

    Bool matchFound=False;

    #ifdef DEBUG
      fprintf(stderr,"reading %s started\n",__func__);
    #endif

    while (! feof(fp)){
      wordBuf = getWord(fp);

      //First letter match
      if (wordBuf != NULL){
	alreadyInStorage = wordInNode(storageNode,query);
	if ((strlen(query)< 2) || (alreadyInStorage == 1)){//Word is in tree 
	  //fprintf(stderr,"%s already in storage\n",query);
	  break;
	}
	if ((FIRST_LETTER_MATCH == True) && 
	    (query[0] != wordBuf[0]))
	  goto freeWordBuf;
	else{
	  int srcLen = strlen(wordBuf)/sizeof(char);
	  int queryLen = strlen(query)/sizeof(char);
	  int wRank = wordSimilarity(query, wordBuf, LEN_MATCH_BOOL);

	  if ((wRank== queryLen) && (srcLen == queryLen)){ 
	    //Absolute match found
	    matchFound = True;
	    break; 
	  }
	  int wordBufLen = strlen(wordBuf)/sizeof(char);
	  if ((wRank > THRESHOLD_RANK) && (wordBufLen > THRESHOLD_LEN)){
	    wordNode = addWord(wordNode,wordBuf,wRank);
	  }
	}
      }
      freeWordBuf:{
	if (wordBuf != NULL){
	  free(wordBuf); 
	  wordBuf = NULL;
	}
      }
    }

    FILE *correctedTxtFP = correctedDest;
    if (correctedTxtFP == NULL)
      correctedTxtFP = stdout;

    fprintf(correctedTxtFP, "%s ",query);

    //If the exact match was found, no need to display the suggested matches
    if ((alreadyInStorage != 1) && (!matchFound)){
      size_t queryLen = strlen(query);

      if (queryLen > 2)
	storageNode = addWord(storageNode,query,queryLen);

      if (nodePrint(correctedTxtFP, wordNode));
      else{
	#ifdef INTERACTIVE
	  fprintf(stderr, "No suggestions\n");
	#endif
      }
    } else { 
      #ifdef INTERACTIVE
      if (alreadyInStorage == 1) fprintf(stderr,"following along %s\n", query);
	fprintf(correctedTxtFP, " in dictionary "); 
      #endif
    }
    fflush(correctedTxtFP);

    if (wordBuf != NULL) free(wordBuf);

    if (wordNode != NULL)
      nodeFree(wordNode);

    return storageNode;
  }

  int wordSimilarity(const char *query, const char *src, Bool LEN_MATCH_TRUE){
    //Determine how much work is required to  transform word 'src' to 'query' 
    //where: 
    //    (nAddedChars*-2)+((nDeletedChars+nMovedChars)*-1)+(nReUsedChars)
    // is the receipe to determine the work done/rank, which is returned
    if ((query == NULL) || (src == NULL)) return 0;

    uint32 queryLen = strlen(query), srcLen = strlen(src),
	  lenSimilarity = ((queryLen == srcLen) ? 1 : 0);

    if ((LEN_MATCH_TRUE == True) && (! lenSimilarity))
      return 0;
 
    numSList *tree = NULL;
    #ifdef TEST
      printf("\033[32mTransforming %s to %s\033[00m\n",src,query);
    #endif

    editStatStruct statStruct;
    initStatStruct(&statStruct);

    tree = wordTransition(query,0,src,tree, &statStruct);
    freeSList(tree);

    int rank = statStructRank(&statStruct);
     
    return rank; 
  }

  void toLower(char *s){
    int len = strlen(s)/sizeof(char);
    int i=0;

    char c;
    while (i<len){
      c = s[i];
      if (isalpha(c)) s[i] = tolower(c); 
      ++i;
    }
  }
  char *getWord(FILE *fp){
    //Copies only alphabetic characters. A non-alphabetic character signals 
    //the function to return the current content
    char *buf = (char*)malloc(sizeof(char)*(BUF_SIZ));
    char c='0';
    int i=0;

    if (buf == NULL){
      fprintf(stderr, "Run out of memory in func %s on line %d in file %s\n",
	  __func__,__LINE__,__FILE__);
      exit(-1);
    }
    while ((!feof(fp)) && (i<BUF_SIZ)){
      if (fread(&c,1,sizeof(char),fp) == -1){
	fprintf(stderr,"Failed to read a char from fp in function %s in file %s\n",
	  __func__, __FILE__);
	exit(-2);
      }
 
      if (! isalpha(c)){//Reading done on finding a non-alphabetic character
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
  int getLine(char *s, int max){
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
#endif
