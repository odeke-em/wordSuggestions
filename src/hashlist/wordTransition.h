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

  void initEditStat(EditStat *est);
  inline EditStat *allocEditStat(void); 

  EditStat *allocAndInitEditStat(void);

  void printStat(const EditStat *);

  int getRank(const char *query, const char *from);
  EditStat *getEditStats(const char *subject, const char *base);
#endif
