#ifndef _CUT_H_
#define _CUT_H_

#include <stdbool.h>
#include "list.h"

typedef struct
{
	// list node of list of graphs
	ListNode* graph;

	// array of booleans defining side of each node
	bool* sideArray;

	// number of required cuts
	int cuts;
} Cut;


// cuts graphs using given findCut function with given parameters
// returns number of successful cuts and saves cutted graphs into list
extern int cutGraphs(List** graphs, int n, double maxDiff, Cut* (*findCut)(ListNode*, double));

// calculates difference between divided graph sizes and checks if it can be divided
// maxDiff shouldn't be in percentages, but percentages/100
extern bool canBeCut(int firstPartSize, int secondPartSize, double maxDiff);

#endif
