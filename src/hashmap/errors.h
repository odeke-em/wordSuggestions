// Author: Emmanuel Odeke <odeke@ualberta.ca>
#ifndef _ERRORS_H
#define _ERRORS_H
  #include <stdio.h>
  #include <stdlib.h>
  #include <stdarg.h>

  typedef enum {
    TypeError, ValueError, IndexError, SyntaxError, BufferOverFlow,
    AssertionError, NullPointerException, IndexOutOfBoundsException,
    ZeroDivisionException, CorruptedDataException
  } Exception;
  
  #define raiseWarning(...){\
    fprintf(stderr, "\033[31m[%s: %s]\033[00m Traceback to line: %d:: ", __FILE__, __func__, __LINE__);\
    fprintf(stderr, __VA_ARGS__);\
  }

  #define throwException(errCode,errMsg){\
    if (errMsg != NULL){\
      fprintf(stderr, "%s\n", #errMsg);\
    }\
    raiseWarning(#errCode);\
    exit(-1);\
  }

  #ifdef assert 
    #undef assert
  #endif // assert

  #define assert(validExpression){\
    if (! (validExpression))\
      raiseError(#validExpression);\
  }

  #define raiseError(...) {\
    raiseWarning(__VA_ARGS__);\
    exit(-2);\
  }

  #define raiseExceptionIfNull(expression){\
    if (! expression)\
      throwException(NullPointerException, expression);\
  }
#endif // _ERRORS_H
