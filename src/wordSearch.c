#include "../include/constants.h"
#include "../include/wordSearch.h"

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

Node *getSuggestions(
  const wordArrayStruct *dictWordArraySt, FILE *correctedDest, 
  Node *storageNode, const word query, const wordMatchCriteria matchCriteria
){
  if ((dictWordArraySt == NULL) || (query == NULL) || (strlen(query) == 0))
    return storageNode;

  Bool alreadyInStorage = False; 
  word wordBuf; 
  Node *wordNode = NULL;

  int nMatches = 0;

  Bool matchFound=False;

#ifdef DEBUG
  fprintf(stderr,"reading %s started\n",__func__);
#endif

  int foundInDict = bSearch(dictWordArraySt, query);
  if (foundInDict != -1){
    matchFound = True;
  }else{
    int maxCutOffRank = wordSimilarity(query, query, matchCriteria.lenMatch_bool);
    int i, nElems = dictWordArraySt->n;
    for (i=0; i<nElems; ++i){
      wordBuf = dictWordArraySt->wordArray[i];

      if ((matchCriteria.firstLetterMatch_bool == True) && (query[0] != wordBuf[0])) continue;

      if (wordBuf != NULL){
        int queryLen = strlen(query)/sizeof(char);
        alreadyInStorage = wordInNode(storageNode,query);
        if ((queryLen < THRESHOLD_LEN) || (alreadyInStorage == True)){
          //This word doesn't need to be added to the tree
          //fprintf(stderr,"%s already in storage\n",query);
          break;
        }

        int wRank = wordSimilarity(query, wordBuf, matchCriteria.lenMatch_bool);
	if (wRank == maxCutOffRank){
	  matchFound= True;
	  break;
	}

	double percentRank = 100*(((double)(wRank))/(double)(maxCutOffRank));
	  
        int wordBufLen = strlen(wordBuf)/sizeof(char);
        if (
	  percentRank >= THRESHOLD_PERCENT_RANK && wordBufLen >= THRESHOLD_LEN
	){
          wordNode = addWord(wordNode, wordBuf, wordBufLen, wRank);
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
      storageNode = addWord(storageNode, query, queryLen, queryLen);

    if (nodePrint(correctedTxtFP, wordNode));
    else{
    #ifdef INTERACTIVE
       fprintf(correctedTxtFP, "\033[31m%s: No suggestions\033[00m\n", query);
    #endif
    }
  } else { 
  #ifdef INTERACTIVE
    if (alreadyInStorage == True); //fprintf(stderr,"following along %s\n", query);
      fprintf(correctedTxtFP, "\033[34m in dictionary \033[00m"); 
  #endif
  }

  fflush(correctedTxtFP);

  nodeFree(wordNode);

  return storageNode;
}

Tree *getMatches(
  const wordArrayStruct *dictWArray, FILE *correctedDest, 
  Tree *memoizeTree, const word query, const wordMatchCriteria matchCriteria, 
  const ElemFuncStruct ElemFuncSt
){
  /*
   Find words whose similarity to the query word is above the threshold 
   match percentage. Add these similar words to the tree 'memoizeTree'
  */
  if ((dictWArray == NULL) || (query == NULL) || (strlen(query) == 0))
    return memoizeTree;

  Bool alreadyInStorage = False; 
  Tree *matches_tree = NULL;

  int nMatches = 0;

  Bool matchFound=False;

#ifdef DEBUG
  fprintf(stderr,"reading %s started\n",__func__);
#endif
  printf("in %s\n", __FILE__);
  TElem queryElem = (TElem)query;
  TElem foundInDict = treeSearch(memoizeTree, queryElem, ElemFuncSt);
  if (foundInDict != NULL_ELEM){
    matchFound = True;
  } else {
    int maxCutOffRank = wordSimilarity(query, query, matchCriteria.lenMatch_bool);
    word wordBuf = NULL; 
    int i, nElems = dictWArray->n;
    for (i=0; i<nElems; ++i){
      wordBuf = dictWArray->wordArray[i];

      if (matchCriteria.firstLetterMatch_bool == True && query[0] != wordBuf[0])
	continue;

      if (wordBuf != NULL) {
        int queryLen = strlen(query)/sizeof(char);
        TElem isMemoized = treeSearch(matches_tree , query, ElemFuncSt);
        if ((queryLen< THRESHOLD_LEN) || isMemoized) {
          //This word doesn't need to be added to the tree
          //fprintf(stderr,"%s already in storage\n",query);
          break;
        }

        int wRank = wordSimilarity(query, wordBuf, matchCriteria.lenMatch_bool);
	if (wRank == maxCutOffRank) {
	  matchFound= True;
	  break;
	}

	double percentRank = 100*(((double)(wRank))/(double)(maxCutOffRank));
	  
        int wordBufLen = strlen(wordBuf)/sizeof(char);
        if  ((percentRank >= THRESHOLD_PERCENT_RANK) 
	 && (wordBufLen >= THRESHOLD_LEN)){
	  TElem isMemoized = treeSearch(matches_tree, wordBuf, ElemFuncSt);
	  if (isMemoized == NULL_ELEM) { //Not stored yet
	    memoizeTree = insert(memoizeTree, wordBuf, ElemFuncSt);

	    //Will implement tree merging => should collect all the 
	    //temporary matches into matches_tree, then finally merge 
	    //them into memoize_tree

	    matches_tree = insert(matches_tree, wordBuf, ElemFuncSt);
	    nMatches += 1;
	  }
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
    if (nMatches) //Add words whose likely corrections were found
      memoizeTree = insert(memoizeTree, query, ElemFuncSt);

    if (serializeTree(matches_tree, correctedTxtFP, BFS_TRAV));
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

  preOrder(matches_tree, stdout);
  printf("Done preordering\n");
  if (matches_tree != NULL) freeTree(matches_tree, ElemFuncSt);

  return memoizeTree;
}
