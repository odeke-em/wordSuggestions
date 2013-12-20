// Author: Emmanuel Odeke <odeke@ualberta.ca>
#ifndef _ASYNC_H
#define _ASYNC_H
  typedef struct {
    void *args;
    void *result;
    void * (*func)(void *);
  } thArg;
#endif
