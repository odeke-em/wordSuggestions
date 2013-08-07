#ifndef _WORK_SPLITTER_H
#define _WORK_SPLITTER_H
  #include <stdio.h>
  #include <stdlib.h>

  #include "customTypes.h"
  #include "utilityFuncs.h"

  //Return the size of a file
  size_t fileSize(FILE *);

  void initNavigator(navigator *);

  navigator *navAlloc(void);
  void navFree(navigator *);

  void setNavigator(navigator *, FILE *, const int *,const int *, const word );
  void printNavigator(navigator *);

  //Paritition a file into n parts and return the list
  //of the navigators to manage those partitions 
  navigatorList *fragmentFile(FILE *, const int *);

  navigatorList *navListAlloc(void);
  void navListFree(navigatorList *);
  void initNavList(navigatorList *, const int *);

  void *cat(void *);
  void *autoC(void *data);
#endif
