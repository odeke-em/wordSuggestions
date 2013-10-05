#include "../include/utilityFuncs.h"

#include <stdio.h>
#include <sys/stat.h>

Bool isValidFile(const word path, long *savFileSize){
  //Input: A path, and a long pointer to save the file size once retrieved
  //Returns: True iff a path exists and is not a directory else False
  struct stat statInfo;
  if (stat(path, &statInfo) != 0) //Path doesn't exist or is null
    return False;

  if ((statInfo.st_mode <= 0))
    return False;

  if (! S_ISDIR(statInfo.st_mode)){
    *savFileSize = statInfo.st_size;
    return True;
  }

  return False;
}
