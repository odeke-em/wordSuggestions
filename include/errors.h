#ifndef _ERRORS_H
#define _ERRORS_H
  #include <stdio.h>
  #define TypeError	  0xf0
  #define ValueError	  0xf1
  #define IndexError	  0xf2
  #define SyntaxError	  0xf3
  #define BufferOverFlow  0xf4
  #define AssertionError  0xf5
  #define NullPointerException  0xf6
  #define OutOfBoundsException  0xf7
  #define ZeroDivisionException 0xf8

  #define CorruptedDataException 0xf9
  
  #define raiseWarning(errMsg){\
    fprintf(stderr,"%s on line %d in function '%s' file '%s'\n",\
      errMsg,__LINE__,__func__,__FILE__);\
  }

  #define throwException(errCode,errMsg){\
    if (errMsg != NULL){\
      fprintf(stderr, "%s\n", errMsg);\
    }\
    raiseWarning(#errCode);\
    exit(-1);\
  }
 
  #define assert(validExpression){\
    if (! validExpression)\
      raiseError(validExpression);\
  }

  #define raiseError(args) {\
    fprintf(stderr, "Traceback most recent call at line: %d ", __LINE__);\
    fprintf(stderr, "of file: %s\nExpression: %s is invalid\n", \
	    __FILE__, #args);\
    exit(-2);\
  }\
   
#endif
