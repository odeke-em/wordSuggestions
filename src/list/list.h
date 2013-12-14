// Author: Emmanuel Odeke <odeke@ualberta.ca>
#ifndef _LIST_H
#define _LIST_H
  typedef enum {
    LESSTHAN=-1, EQUAL=0, GREATERTHAN=1
  } Comparison;

  typedef struct Node_ {
    struct Node_ *next;
    void *data;
    unsigned int tag:1;
  } Node;

  typedef struct List_ {
    int size;
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
  inline int getListSize(const List *l);

  List *append(List *l, void *data);
  Node *find(List *l, void *query, Comparator comp);
  List *removeElem(List *l, void *query, Comparator comp);

  void destroyList(List *l);

  // Miscellaneous
  void printList(List *l);
  Comparison intPtrComp(const void *i1, const void *i2);
#endif
