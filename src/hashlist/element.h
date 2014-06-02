#ifndef _ELEMENT_H
#define _ELEMENT_H
    #ifndef _BOOL_D
    #define _BOOL_D
    typedef enum {
        False=0, True=1
    } Bool;
    #endif // _BOOL_H

    typedef struct MetaRankMule_ {
        int rank;
        int metaInfo;
        int hash;
    } MetaRankMule;

    typedef struct Element_ {
        Bool dTag; // Discovery tag
        void *value;
        double rank;
        int metaInfo;
        struct Element_ *next;
    } Element;

    inline Bool hasNext(Element *e);
    inline Element *getNext(Element *e);

    Element *addToTail(Element *sl, void *data);
    Element *addToTailWithMetaInfo(Element *sl, void *data, const int metaInfo);
    Element *addToHeadWithRank(Element *sl, void *data, const double rank);
    Element *addToHead(Element *sl, void *data);
    Element *initElement(Element *elem);
    long int destroySList(Element *sl);

#endif // _ELEMENT_H
