// Author: Emmanuel Odeke <odeke@ualberta.ca>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "hashList.h"
#include "wordTransition.h"

typedef struct {
    char *query;
    unsigned int ownRank;
    Element **start, **end;
    double thresholdPercentage;
} FuncWrapper;

typedef struct {
    unsigned int n;
    Element **elemList;
} ElementList;

Element *matchByThreshold(const void *data) {
    if (data != NULL) {
        FuncWrapper *rStruct = (FuncWrapper *)data;
        Element *resultSL = NULL, **trav=rStruct->start, **end=rStruct->end;
        while (trav < end) {
            int rank = getRank(rStruct->query, (*trav)->value);
            if (rank >= rStruct->thresholdPercentage) {
                resultSL = addToHeadWithRank(resultSL, (*trav)->value, (double)rank/rStruct->ownRank);
            }
            ++trav;
        }

        return resultSL;
    }
    else {
        return NULL;
    }
}

Element *matchesByThreshold(
    const char *query, HashList *dict, const unsigned int ownRank,
    const double threshold, unsigned int thCount
) {
    Element *mergedSL= NULL;
    if (dict != NULL && dict->size >= 1 && query != NULL) {
        thCount = thCount ? thCount: 2;
        unsigned long int stepCount = dict->size/thCount;
        pthread_t thList[thCount];
        FuncWrapper fwrapperL[thCount];
    }

    return mergedSL;
}

#ifdef REV_MAIN_RANKING
int main(int argc, char *argv[]) {
    return 0;
}
#endif
