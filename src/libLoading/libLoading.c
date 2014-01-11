// Author: Emmanuel Odeke <odeke@ualberta.ca>
// Module to handle platform agnostic loading of dynamic libraries

#include <stdio.h>
#include <stdarg.h>
#include "libLoading.h"

#ifdef WIN32
  #include <windows.h>
#else
  #include <dlfcn.h>
#endif

typedef char *(*errorReporter)(void);
typedef int (*libCloser)(void *handle);

static errorReporter getErrorReporter(void) {
  static errorReporter errReporter = NULL;
#ifdef WIN32
  char *win32DLError(void) {
    // Source:: Armin Rigo <arigo@tunes.org> from Python commit
    static char buf[32];
    DWORD dw = GetLastError();
    if (dw == 0) 
      return NULL;

    sprintf(buf, "error 0x%x", (unsigned int)dw);
    return buf;
  }

  errReporter = win32DLError;
#else
  errReporter = dlerror;
#endif

  return errReporter;
}

void *libloader(const char *libName, ...) {
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

int main() {
  errorReporter er = getErrorReporter();
  printf("er: %p\n", er);
  return 0;
}
