#ifndef _UTILITY_FUNCS_H
#define _UTILITY_FUNCS_H
  #include <stdio.h>
  #include <sys/stat.h>

  #include "customTypes.h"

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
#endif
