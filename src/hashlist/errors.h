#ifndef _ERRORS_H
#define _ERRORS_H
  #include <stdio.h>

  typedef enum {
    TypeError, ValueError, IndexError, SyntaxError, BufferOverFlow,
    AssertionError, NullPointerException, IndexOutOfBoundsException,
    ZeroDivisionException, CorruptedDataException
  } Exception;
  
  #define raiseWarning(errMsg){\
    fprintf(stderr,"\033[31m%s on line %d in function '%s' file '%s'\033[00m\n",\
      errMsg,__LINE__,__func__,__FILE__);\
  }

  #define throwException(errCode,errMsg){\
    if (errMsg != NULL){\
      fprintf(stderr, "%s\n", #errMsg);\
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
    fprintf(stderr, "of file: %s\n\033[31m%s\033[00m\n", \
         __FILE__, #args);\
    exit(-2);\
  }

  #define raiseExceptionIfNull(expression){\
    if (! expression)\
      throwException(NullPointerException, expression);\
  }
#endif
