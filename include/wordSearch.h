/*
  Author: Emmanuel Odeke <odeke@ualberta.ca>
*/
#ifndef _WORD_SEARCH_H
#define _WORD_SEARCH_H
  #include <stdio.h>
  #include <unistd.h>
  #include <stdlib.h>
  #include <string.h>

  #include "Node.h"
  #include "utilityFuncs.h"
  #include "wordTransition.h"
  #include "wordLib.h"

  #define DICTIONARY_PATH "../resources/wordlist.txt"
  #define THRESHOLD_LEN  2 //Arbitrary value here
  #define THRESHOLD_PERCENT_RANK 70 //To be auto-calibrated in the future

  int wordSimilarity(const word query, const word src, const Bool LEN_MATCH_TRUE){
    //Determine how much work is required to  transform word 'src' to 'query' 
    //where: 
    // rank = (inplace*3)+(moves*2)+(deletions*-1)+(additions*-1);

    if ((query == NULL) || (src == NULL)) return 0;

    uint32 queryLen = strlen(query), srcLen = strlen(src),
      lenSimilarity = ((queryLen == srcLen) ? True : False);

    if ((LEN_MATCH_TRUE == True) && (! lenSimilarity))
      return 0;
 
    #ifdef TEST
      printf("\033[32mTransforming %s to %s\033[00m\n",src,query);
    #endif

    editStatStruct *statStruct = wordTranspose(query,src);

    int rank = statStructRank(statStruct);
     
    return rank; 
  }

  Node *loadWord(
    const wordArrayStruct *wArrSt, FILE *correctedDest, Node *storageNode,
    const word query, const Bool LEN_MATCH_BOOL, const Bool FIRST_LETTER_MATCH
  ){
    /*
      Find words whose similarity to the query word is above the threshold 
      match percentage. Add these similar words to the singly linked list:
      'storageNode'.
    */
    if ((wArrSt == NULL) || (query == NULL) || (strlen(query) == 0))
      return storageNode;

    Bool alreadyInStorage = False; 
    word wordBuf; 
    Node *wordNode = NULL;

    int nMatches = 0;

    Bool matchFound=False;

    #ifdef DEBUG
      fprintf(stderr,"reading %s started\n",__func__);
    #endif

    int foundInDict = bSearch(wArrSt, query);
    if (foundInDict != -1){
      matchFound = True;
    }else{
      int maxCutOffRank = wordSimilarity(query, query, LEN_MATCH_BOOL);
      int i, nElems = wArrSt->n;
      for (i=0; i<nElems; ++i){
        wordBuf = wArrSt->wordArray[i];

	if ((FIRST_LETTER_MATCH == True) && (query[0] != wordBuf[0])) continue;
        if (wordBuf != NULL){
          int queryLen = strlen(query)/sizeof(char);
          alreadyInStorage = wordInNode(storageNode,query);
          if ((queryLen< THRESHOLD_LEN) || (alreadyInStorage == True)){
            //This word doesn't need to be added to the tree
            //fprintf(stderr,"%s already in storage\n",query);
            break;
          }

          int wRank = wordSimilarity(query, wordBuf, LEN_MATCH_BOOL);
	  if (wRank == maxCutOffRank){
	    matchFound= True;
	    break;
	  }
	  double percentRank = 100*(((double)(wRank))/(double)(maxCutOffRank));
	  
          int wordBufLen = strlen(wordBuf)/sizeof(char);
          if ((percentRank >= THRESHOLD_PERCENT_RANK) && \
	    (wordBufLen >= THRESHOLD_LEN)){
            wordNode = addWord(wordNode,wordBuf,wRank);
            nMatches += 1;
          }
        }
      }
    }

    FILE *correctedTxtFP = correctedDest;
    if (correctedTxtFP == NULL)
      correctedTxtFP = stdout;

    fprintf(correctedTxtFP, "%s ",query);

    //If the exact match was found, no need to display the suggested matches
    if ((alreadyInStorage != True) && (!matchFound)){
      size_t queryLen = strlen(query);

      if (nMatches) //Add words whose likely corrections were found
        storageNode = addWord(storageNode,query,queryLen);

      if (nodePrint(correctedTxtFP, wordNode));
      else{
        #ifdef INTERACTIVE
          fprintf(stderr, "No suggestions\n");
        #endif
      }
    } else{ 
      #ifdef INTERACTIVE
      if (alreadyInStorage == 1) fprintf(stderr,"following along %s\n", query);
        fprintf(correctedTxtFP, "\033[34m in dictionary \033[00m"); 
      #endif
    }
    fflush(correctedTxtFP);

    if (wordNode != NULL) nodeFree(wordNode);

    return storageNode;
  }
#endif
