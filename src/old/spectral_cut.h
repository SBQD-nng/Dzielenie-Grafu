#ifndef _SPECTRAL_CUT_H_
#define _SPECTRAL_CUT_H_

#include "../graph.h"
#include "../cut.h"

// find cut using spectral algorithm; it needs to be in maxDiff range, which should be provided in percentages
// returns NULL when it isn't possible
extern Cut* findSpectralCut(ListNode* listNode, double maxDiff);

#endif
