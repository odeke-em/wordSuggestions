#include <stdlib.h>
#include "element.h"

inline Bool hasNext(Element *e) { return e != NULL && e->next != NULL; }
inline Element *getNext(Element *e) { return e == NULL ? NULL : e->next; }

Element *addToTail(Element *sl, void *data) {
    sl = addToTailWithMetaInfo(sl, data, 0);
    return sl;
}

Element *addToTailWithMetaInfo(Element *sl, void *data, const int metaInfo) {
    if (sl == NULL) {
	sl = initElement(sl);
	sl->value = data;
	sl->metaInfo = metaInfo;
    } else {
        sl->next = initElement(sl->next);
        sl->value = data;
        sl->metaInfo = metaInfo;
        sl->next = sl->next->next;
    }

    return sl;
}

Element *addToHeadWithRank(Element *sl, void *data, const double rank) {
    sl = addToHead(sl, data);
    if (sl != NULL) {
        sl->rank = rank;
    }

    return sl;
}

Element *addToHead(Element *sl, void *data) {
    if (sl == NULL) {
        sl = initElement(sl);
        sl->value = data;
    } else {
        Element *newElem = NULL;
        newElem = initElement(newElem);
        newElem->value = data;
        newElem->next = sl;
        sl = newElem;
    }

    return sl;
}

Element *initElement(Element *elem) {
    if (elem == NULL) {
        elem = (Element *)malloc(sizeof(Element));
    } 

    elem->next = NULL;
    elem->value = NULL;
    elem->rank = 1; // Iniitally rank is at 100%
    elem->metaInfo = 0;
    elem->dTag = False; // Hasn't been discovered

    return elem;
}

long int destroySList(Element *sl) {
    long int nValueFrees = 0;
    if (sl != NULL) { 
	// printf("Sl == NULL: %d\n", sl != NULL);
	Element *tmp;
	while (sl != NULL) { 
	    tmp = sl->next;
	#ifdef DEBUG
            printf("Freeing:: curHead: %p Next: %p\n", sl, tmp);
	#endif
            if (sl->value != NULL) {
	        free(sl->value);
	        ++nValueFrees;
	    }

	    free(sl);

	    sl = tmp;
	}
        sl = NULL;
    }

    return nValueFrees;
}
