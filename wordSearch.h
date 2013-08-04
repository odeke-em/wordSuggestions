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

  #define THRESHOLD_RANK 2
  #define THRESHOLD_LEN  2
  
  int wordSimilarity(const word, const word, Bool );
  Node *loadWord(FILE *fp, FILE *correctedDest, Node *storageNode,
    const word query, Bool LEN_MATCH_BOOL, Bool FIRST_LETTER_MATCH){
    /*
      Find words whose similarity to the query word is above the threshold 
      match percentage. Add these similar words to the singly linked list:
      'storageNode'.
    */
    if ((fp == NULL) || (query == NULL) || (strlen(query) == 0))
      return storageNode;

    //Rewind file to the very beginning
    fseek(fp, 0, SEEK_SET);
    Bool alreadyInStorage = False; 
    word wordBuf = NULL; 
    Node *wordNode = NULL;

    int nMatches = 0;

    Bool matchFound=False;

    #ifdef DEBUG
      fprintf(stderr,"reading %s started\n",__func__);
    #endif

    while (! feof(fp)){
      wordBuf = getWord(fp);
      skipTillCondition(fp, notSpace);
      //First letter match
      if (wordBuf != NULL){
        alreadyInStorage = wordInNode(storageNode,query);
        if ((strlen(query)< THRESHOLD_LEN) || (alreadyInStorage == True)){
         //This word doesn't need to be added to the tree
         //fprintf(stderr,"%s already in storage\n",query);
         break;
        }

        if ((FIRST_LETTER_MATCH == True) && (query[0] != wordBuf[0]))
          goto freeWordBuf;
        else{
          int srcLen = strlen(wordBuf)/sizeof(char);
          int queryLen = strlen(query)/sizeof(char);
          int wRank = wordSimilarity(query, wordBuf, LEN_MATCH_BOOL);

          if ((srcLen == queryLen) && (wRank == queryLen<<1)){ 
            //Absolute match found
            matchFound = True;
            break; 
          }
          int wordBufLen = strlen(wordBuf)/sizeof(char);
          if ((wRank > THRESHOLD_RANK) && (wordBufLen > THRESHOLD_LEN)){
            wordNode = addWord(wordNode,wordBuf,wRank);
            nMatches += 1;
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
        fprintf(correctedTxtFP, " in dictionary "); 
      #endif
    }
    fflush(correctedTxtFP);

    if (wordBuf != NULL) free(wordBuf);

    if (wordNode != NULL) nodeFree(wordNode);

    return storageNode;
  }

  int wordSimilarity(const word query, const word src, Bool LEN_MATCH_TRUE){
    //Determine how much work is required to  transform word 'src' to 'query' 
    //where: 
    //    (nAddedChars*-2)+((nDeletedChars+nMovedChars)*-1)+(nReUsedChars)
    // is the receipe to determine the work done/rank, which is returned
    if ((query == NULL) || (src == NULL)) return 0;

    uint32 queryLen = strlen(query), srcLen = strlen(src),
      lenSimilarity = ((queryLen == srcLen) ? True : False);

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
#endif
