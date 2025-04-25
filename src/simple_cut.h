#ifndef _SIMPLE_CUT_H_
#define _SIMPLE_CUT_H_

#include "graph.h"
#include "cut.h"

// find simple cut that requires lowest number of cuts for given graph
// and is in maxDiff range (maxDiff shouldn't be in percentages, but percentages/100)
// returns NULL when it isn't possible
extern Cut* findSimpleCut(ListNode* listNode, double maxDiff);

#endif
