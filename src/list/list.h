// Author: Emmanuel Odeke <odeke@ualberta.ca>
#ifndef _LIST_H
#define _LIST_H
  #include <stdarg.h>
  typedef enum {
    Less=-1, Equal=0, Greater=1
  } Comparison;

  typedef enum Bit_ {
    Unknown=-1, Stackd=0, Heapd=1
  } Tag;

  typedef struct Node_ {
    Tag tag;
    void *data;
    struct Node_ *next;
    int (*freeData)(void *data);
  } Node;

  typedef struct List_ {
    unsigned int size;
    Node *head, *tail;
  } List;

  typedef Comparison (*Comparator)(const void *, const void *);

  Node *initNode(Node *);
  Node *createNewNode(void);
  inline Node *allocNode(void);

  List *initList(List *);
  List *createNewList(void);
  inline List *allocList(void);

  inline void *getData(const Node *n);
  inline void *getNextNode(const Node *n);
  inline Tag getTag(const Node *n);
  inline unsigned int getListSize(const List *l);

  void *listPop(Node **n);
  void *popHead(List *l);
  inline void *peek(const List *l);

  List *appendAndTag(List *l, void *data, Tag tag, void (*freer)(void *));
  List *appendWithFreer(List *l, void *data, void (*freer)(void *));

  // If no tag is explicitly provided, it is assumed that data is memory
  // obtained by a call to malloc or an object that exists on the heap
  List *append(List *l, void *data);

  List *prependAndTag(List *l, void *data, Tag tag);

   // If no tag is explicitly provided, it is assumed that data is memory
  // obtained by a call to malloc or an object that exists on the heap
  List *prepend(List *l, void *data);

  Node *find(List *l, void *query, Comparator comp);
  List *removeElem(List *l, void *query, Comparator comp);

  inline int isEmpty(const List *l);
  void destroyList(List *l);

  // Miscellaneous
  void printList(List *l);
  Comparison intPtrComp(const void *i1, const void *i2);

  // Pass the number of lists to merge, the lists themselves and
  // the very last argument should be the Comparator
  List *multiMerge(unsigned int count, ...);

  void swap(void **a, void **b);
#endif
