// Author: Emmanuel Odeke <odeke@ualberta.ca>
#ifndef _RADLOADWORDS_H
#define _RADLOADWORDS_H
    #include "element.h"

    typedef Element LinearizedTrie;
    RTrie *fileToRTrie(const char *filePath);

    Element *getCloseMatches(const char *query, RTrie *dict, const double percentMatch);
    Element *matches(const char *query, RTrie *dict, const unsigned int r, const double percentMatch);

    void printLinearizedTrie(LinearizedTrie *lt);
    LinearizedTrie *linearizeRTrie(RTrie *rt, LinearizedTrie *mp);
    LinearizedTrie *destroyLinearizedTrie(LinearizedTrie *lt);
#endif // _RADLOADWORDS.H
