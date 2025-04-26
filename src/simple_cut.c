#include "simple_cut.h"
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include "array.h"
#include "graph.h"
#include "list.h"
#include "cut.h"


// recursively marks all connected nodes on the same side
static void markAllConns(Node* node);


Cut* findSimpleCut(ListNode* listNode, double maxDiff, int verifyConnectivity)
{
	Graph* graph = listNode->val;
	int nodeCount = graph->nodes->len;
	Node** nodes = (Node**)graph->nodes->arr;

	Cut* cut = malloc(sizeof(Cut));
	cut->graph = listNode;
	cut->sideArray = malloc(sizeof(bool) * nodeCount);
	cut->cuts = INT_MAX;

	for (int i = 0; i < nodeCount; i++)
	{
		nodes[i]->simpleCut_secondPart = false;
	}
	int secondPartSize = 0;

	while (1)
	{
		int firstPartSize = nodeCount - secondPartSize;
		Node* anyFromFirst = NULL;
		Node* anyFromSecond = NULL;

		if (canBeCut(firstPartSize, secondPartSize, maxDiff))
		{
			// count number of cuts for a given node configuration
			int cuts = 0;
			for (int i = 0; i < nodeCount; i++)
			{
				// reset marks
				nodes[i]->mark = false;

				if (nodes[i]->simpleCut_secondPart)
				{
					anyFromSecond = nodes[i];
					// if it's on the second side, skip it, so it won't be counted two times
					continue;
				}
				else
				{
					anyFromFirst = nodes[i];
				}

				ListNode* conns = nodes[i]->conns->first;
				while (conns != NULL)
				{
					// nodes[i] are always on the first side, so if it connects to second side, increment cuts
					if (((Node*)conns->val)->simpleCut_secondPart) { cuts++; }
					conns = conns->next;
				}
			}

			// if new lowest number of cuts, verify connections and copy if valid
			if (cuts < cut->cuts)
			{
				bool validGraph = true;

				if (verifyConnectivity != 0)
				{
					markAllConns(anyFromFirst);
					markAllConns(anyFromSecond);

					for (int i = 0; i < nodeCount; i++)
					{
						// if it wasn't marked it means it's separated from the rest of graph, so it isn't valid graph
						if (!nodes[i]->mark)
						{
							validGraph = false;
							break;
						}
					}
				}

				if (validGraph)
				{
					// copy configuration to SimpleCut structure
					for (int i = 0; i < nodeCount; i++)
					{
						cut->sideArray[i] = nodes[i]->simpleCut_secondPart;
					}
					cut->cuts = cuts;
				}
			}
		}

		bool finishLoop = false;
		for (int i = 0;; i++)
		{
			// we treat nodes as bit counter and increment it's value by 1
			if (nodes[i]->simpleCut_secondPart)
			{
				nodes[i]->simpleCut_secondPart = false;
				secondPartSize--;
			}
			else
			{
				nodes[i]->simpleCut_secondPart = true;
				secondPartSize++;
				break; // if bit went from 0 to 1 exit, if from 1 to 0 swap next bit
			}

			// if it wants to swap last bit (nodeCount - 1) it means all configurations were checked
			// we don't want to check configurations with last bit set to 1, as they can be paired with their negations
			// so we would just check first side swapped with second
			if (i >= nodeCount - 2)
			{
				finishLoop = true;
				break;
			}
		}
		if (finishLoop) break;
	}

	return cut->cuts == INT_MAX ? NULL : cut;
}

void markAllConns(Node* node)
{
	node->mark = true;
	bool secondPart = node->simpleCut_secondPart;

	ListNode* conns = node->conns->first;
	while (conns != NULL)
	{
		Node* node2 = (Node*)conns->val;

		// mark only if it's on the same side and wasn't yet marked
		if (secondPart == node2->simpleCut_secondPart && !node2->mark)
		{
			markAllConns(node2);
		}

		conns = conns->next;
	}
}
