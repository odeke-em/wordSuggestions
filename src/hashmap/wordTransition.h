// Author: Emmanuel Odeke <odeke@ualberta.ca>

#ifndef _WORD_TRANSITION_H
#define _WORD_TRANSITION_H
  typedef unsigned int uint32;
  typedef struct {
    uint32 moves, 
	  reuses,
	  inplace,
	  additions,
	  deletions,
	  stringLen;
  } EditStat;

  typedef struct IndexNode_ {
    int index;
    struct IndexNode_ *next;
  } IndexNode;

  void initEditStat(EditStat *est);
  inline EditStat *allocEditStat(void); 

  EditStat *allocAndInitEditStat(void);

  void printStat(const EditStat *);
  void printIndexNode(IndexNode **r);

  int getRank(const char *query, const char *from);
  EditStat *getEditStats(const char *subject, const char *base);
#endif
