#include "simpleCut.h"
#include <limits.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "array.h"
#include "graph.h"
#include "list.h"
#include <stdio.h>


typedef struct
{
	// graph to cut
	Graph* graph;

	// array of booleans defining side of each node
	bool* sideArray;

	// number of required cuts
	int cuts;
} SimpleCut;


// find simple cut that requires lowest number of cuts for given graph
// and is in maxDiff range (maxDiff shouldn't be in percentages, but percentages/100)
static SimpleCut* findSimpleCut(Graph* graph, double maxDiff);

// perform simple cut on graph - one part is saved back to input graph and second is returned
static Graph* simpleCut(SimpleCut* cut);

// calculates difference between divided graph sizes and checks if it can be divided
// maxDiff shouldn't be in percentages, but percentages/100
static bool canBeCut(int firstPartSize, int secondPartSize, double maxDiff);

// adds cut to sorted list of cuts
static void addCutToList(List* list, SimpleCut* cut);

// recursively marks all connected nodes on the same side
static void markAllConns(Node* node);


int simpleCutGraphs(Array* graphs, int n, double maxDiff)
{
	if (n < 1) return 0;
	maxDiff /= 100.0;

	List* cutList = list_new();
	for (int i = 0; i < graphs->len; i++)
	{
		SimpleCut* cut = findSimpleCut(array_get(graphs, i), maxDiff);
		if (cut != NULL) addCutToList(cutList, cut);
	}

	for (int i = 0;; i++)
	{
		if (cutList->first == NULL) return i; // return i if there aren't any valid cuts available

		// perform simple cut and add new graph to graphs
		SimpleCut* cut = (SimpleCut*)cutList->first->val;
		Graph* firstGraph = cut->graph;
		Graph* secondGraph = simpleCut(cut);
		array_add(graphs, secondGraph);

		// free cut and remove it from cutList
		free(cut->sideArray);
		free(cut);
		list_remove(cutList, cutList->first);

		if (i == n - 1)
		{
			// exit if it performed n cuts
			break;
		}
		else
		{
			// check new graphs for cuts and add to cutList if valid
			SimpleCut* newCut1 = findSimpleCut(firstGraph, maxDiff);
			SimpleCut* newCut2 = findSimpleCut(secondGraph, maxDiff);
			if (newCut1 != NULL) addCutToList(cutList, newCut1);
			if (newCut2 != NULL) addCutToList(cutList, newCut2);
		}
	}
	return n;
}

SimpleCut* findSimpleCut(Graph* graph, double maxDiff)
{
	int nodeCount = graph->nodes->len;
	Node** nodes = (Node**)graph->nodes->arr;

	// find highest id value
	int maxId = 0;
	for (int i = 0; i < nodeCount; i++)
	{
		if (nodes[i]->id > maxId) maxId = nodes[i]->id;
	}

	SimpleCut* cut = malloc(sizeof(SimpleCut));
	cut->graph = graph;
	cut->sideArray = malloc(sizeof(bool) * (maxId + 1));
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
				nodes[i]->simpleCut_mark = false;

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
					if (((Node*)conns->val)->simpleCut_secondPart) cuts++;
					conns = conns->next;
				}
			}

			// if new lowest number of cuts, verify connections and copy if valid
			if (cuts < cut->cuts)
			{
				markAllConns(anyFromFirst);
				markAllConns(anyFromSecond);

				bool validGraph = true;
				for (int i = 0; i < nodeCount; i++)
				{
					// if it wasn't marked it means it's separated from the rest of graph, so it isn't valid graph
					if (!nodes[i]->simpleCut_mark)
					{
						validGraph = false;
						break;
					}
				}

				if (validGraph)
				{
					// copy configuration to SimpleCut structure
					for (int i = 0; i < nodeCount; i++)
					{
						cut->sideArray[nodes[i]->id] = nodes[i]->simpleCut_secondPart;
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

Graph* simpleCut(SimpleCut* cut)
{
	Graph* graph = cut->graph;
	int oldSize = graph->nodes->len;
	Node** oldNodes = (Node**)graph->nodes->arr;

	Array* array1 = array_create(sizeof(Node*), true);
	Array* array2 = array_create(sizeof(Node*), true);

	for (int i = 0; i < oldSize; i++)
	{
		bool secondPart = cut->sideArray[oldNodes[i]->id];
		array_add(secondPart ? array2 : array1, oldNodes[i]);

		ListNode* conns = oldNodes[i]->conns->first;
		while (conns != NULL)
		{
			ListNode* nextConn = conns->next; // preventing read after free
			if (cut->sideArray[((Node*)conns->val)->id] != secondPart)
			{
				list_remove(oldNodes[i]->conns, conns);
			}
			conns = nextConn;
		}
	}

	array_free(graph->nodes);
	graph->nodes = array1;

	Graph* newGraph = malloc(sizeof(Graph));
	newGraph->nodes = array2;
	return newGraph;
}

bool canBeCut(int firstPartSize, int secondPartSize, double maxDiff)
{
	if (firstPartSize == 0 || secondPartSize == 0) return false;
	double a = firstPartSize, b = secondPartSize;

	return fabs((a - b) / ((a + b) / 2.0)) <= maxDiff;
}

void addCutToList(List* list, SimpleCut* cut)
{
	ListNode* node = list->first;
	while (node != NULL)
	{
		if (((SimpleCut*)node->val)->cuts > cut->cuts)
		{
			list_insert(list, node->prev, cut);
			return;
		}
		node = node->next;
	}

	// if it wasn't instered yet it means cut->cuts is higher then all other cuts
	// so append it on a tail of list
	list_append(list, cut);
}

void markAllConns(Node* node)
{
	node->simpleCut_mark = true;
	bool secondPart = node->simpleCut_secondPart;

	ListNode* conns = node->conns->first;
	while (conns != NULL)
	{
		Node* node2 = (Node*)conns->val;

		// mark only if it's on the same side and wasn't yet marked
		if (secondPart == node2->simpleCut_secondPart && !node2->simpleCut_mark)
		{
			markAllConns(node2);
		}

		conns = conns->next;
	}
}
