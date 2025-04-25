#ifndef _KARGER_CUT_H_
#define _KARGER_CUT_H_

#include "cut.h"


// find cut using Karger algorithm; it needs to be in maxDiff range, which should be provided in percentages
// iterations determines number of iterations of this algorithm; returns NULL when it isn't possible to split graph
extern Cut* findKargerCut(ListNode* listNode, double maxDiff);

#endif
