// Author: Emmanuel Odeke <odeke@ualberta.ca>
// Module to handle platform agnostic loading of dynamic libraries

#include <stdio.h>
#include <stdlib.h> // For exit(...)
#include <stdarg.h>
#include "libLoading.h"

static errorReporter errReporter = NULL;

errorReporter getErrorReporter(void) {
  errorReporter errR = NULL;
#ifdef WIN32
  char *win32DLError(void) {
    // Source:: Armin Rigo <arigo@tunes.org> from PyPy commit
    static char buf[32];
    DWORD dw = GetLastError();
    if (dw == 0) 
      return NULL;

    sprintf(buf, "error 0x%x", (unsigned int)dw);
    return buf;
  }

  errR = win32DLError;
#else
  errR = dlerror;
#endif

  return errR;
}

void *libLoader(const char *libName, ...) {
  void *handle = NULL;
#ifdef WIN32
  handle = LoadLibrary(libName);
#else
  va_list ap;
  va_start(ap, libName);
  int flag = va_arg(ap, int);
  handle = dlopen(libName, flag);
  va_end(ap);
#endif

  return handle;
}

void *loadSymbol(void *handle, const char *symName) {
  if (handle != NULL) {
  #ifdef WIN32
    return GetProcAddress(handle, symName);
  #else
    return dlsym(handle, symName);
  #endif
  } else {
    return NULL;
  }
}

void closeLib(void *handle) {
  if (handle != NULL) {
  #ifdef WIN32
    FreeLibrary(handle);
  #else
    dlclose(handle);
  #endif
  }
}

char *getLastError() {
  if (errReporter == NULL) {
    errReporter = getErrorReporter();
  }

  return errReporter();
}

inline void *lazyLoad(const char *libName) {
  int flag = 0;
#ifndef WIN32
  flag = RTLD_LAZY;
#endif
  return libLoader(libName, flag);
}

void checkLoading(void *handle, void *funcPtr, const char *libKey) {
  funcPtr = loadSymbol(handle, libKey);
  char *err = getLastError();
  if (err != NULL) {
    fprintf(stderr, "\033[31m%s\033[00m\n", err);
    exit(-1);
  }
}

#ifdef SAMPLE_RUN
int main() {
  errorReporter er = getErrorReporter();
  printf("er: %p\n", er);
  void *handle = lazyLoad("../exec/libaCorrect.so.1");
  printf("handle: %p\n", handle);
  void *tp = NULL;
  checkLoading(handle, tp, "grocery");
  closeLib(handle);
  return 0;
}
#endif
