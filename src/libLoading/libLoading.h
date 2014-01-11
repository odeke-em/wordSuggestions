// Author: Emmanuel Odeke <odeke@ualberta.ca>
// Module to handle platform agnostic loading of dynamic libraries

#ifndef _LIB_LOADING_H
#define _LIB_LOADING_H
  #include <stdarg.h>

  #ifdef WIN32
    #include <windows.h>
  #else
    #include <dlfcn.h>
  #endif

  typedef char *(*errorReporter)(void);
  typedef int (*libCloser)(void *handle);

  void *libLoader(const char *libName, ...);
  inline void *lazyLoad(const char *libName);
  void *loadSymbol(void *handle, const char *symName);

  static errorReporter getErrorReporter(void);

  void closeLib(void *handle);
#endif
