#include "karger_cut.h"
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include "array.h"
#include "cut.h"
#include "graph.h"
#include "list.h"
#include "errors.h"


typedef struct
{
	// position of structure in array of merged nodes
	int pos;

	// list of real nodes that were merged into this node
	List* realNodes; // of Node*

	// list of connections to other merged nodes
	// it may contain duplicats, which is part of how this algorithm works
	List* conns; // of MergedNode*

	// true if connected with given merged node, false in other case
	bool* connectedWith;

	// used to check if connections to node were already
	// redirected in given iteration of mergind nodes
	// should be initialized to -1
	int mergeId;
} MergedNode;

typedef struct
{
	MergedNode* a;
	MergedNode* b;
	int toCut; // number of required cuts; same as a->conns->size and b->conns->size
} MergedPair;


// iteration of Karger's algorithm
static MergedPair kargerCutIter(Graph* graph);

// copy graph structure to array of MergedNode
static Array* copyGraph(Graph* graph);

// creates and initializes fields of MergedNode sructure when copying a graph
// note that it doesn't fill array of realConns, only initializes it
static MergedNode* initMergedNode(Node* realNode, int pos, int nodeCount);

// merges two nodes; node b should be removed from list after merging (function will free its memory)
static void mergeNodes(MergedNode* a, MergedNode* b, int mergeId);

// removes connections to given node from given connection list
// note that it doesn't modify toNode->conns, so it should be called two times
static void removeConnsTo(MergedNode* fromNode, MergedNode* toNode);

// redirects all connections from conns from redirectFrom to redirectTo
// for performance reasons it's recommended to check and set mergeId to avoid
// calling this function multiple times for single MergedNode in given merging iteration
static void redirectConns(MergedNode* node, MergedNode* redirectFrom, MergedNode* redirectTo);

// removes memory of merged node
static void freeMergedNode(MergedNode* node);

// returs proper number of iterations number from iterations argument
// if iterations > 0 it will return it, else it will calculate it based on graph node count
static int getIterations(int iter, int nodeCount);


Cut* findKargerCut(ListNode* listNode, double maxDiff, int iter)
{
	Graph* graph = listNode->val;
	int nodeCount = graph->nodes->len;
	if (nodeCount < 2) { return NULL; }
	iter = getIterations(iter, nodeCount);

	MergedPair minCut;
	int minCutCount = INT_MAX;

	for (int i = 0; i < iter; i++)
	{
		MergedPair results = kargerCutIter(graph);
		int aSize = results.a->realNodes->size;
		int bSize = results.b->realNodes->size;

		// if new cut requires less removed edges and is in maxDiff
		if (results.toCut < minCutCount && canBeCut(aSize, bSize, maxDiff))
		{
			// free old merged pairs if it wasn't first cut
			if (minCutCount != INT_MAX)
			{
				freeMergedNode(minCut.a);
				freeMergedNode(minCut.b);
			}

			minCut = results;
			minCutCount = results.toCut;
		}
	}

	// return NULL if it didn't find any cut
	if (minCutCount == INT_MAX) { return NULL; }

	Cut* cut = malloc(sizeof(Cut));
	cut->graph = listNode;
	cut->cuts = minCutCount;

	// allocate sideArray and initialize all values set to 0 (false)
	cut->sideArray = calloc(nodeCount, sizeof(bool));

	// set second sides
	ListNode* cutListNode = minCut.b->realNodes->first;
	while (cutListNode != NULL)
	{
		cut->sideArray[((Node*)cutListNode->val)->pos] = true;
		cutListNode = cutListNode->next;
	}

	return cut;
}

MergedPair kargerCutIter(Graph* graph)
{
	Array* array = copyGraph(graph);
	MergedNode** mergedNodes = array->arr;
	int mergeId = 0;

	// repeat until there are only two nodes left
	while (array->len > 2)
	{
		MergedNode* node1 = mergedNodes[rand() % array->len];
		MergedNode* node2 = mergedNodes[rand() % array->len];

		if (node1->connectedWith[node2->pos])
		{
			int oldPos = node2->pos;
			mergeNodes(node1, node2, mergeId);

			// remove node2 from array by replacing it with last array element (if it isn't last element)
			if (oldPos != array->len - 1)
			{
				mergedNodes[oldPos] = mergedNodes[array->len - 1];
				mergedNodes[oldPos]->pos = oldPos;

				// swap values in connectedWith arrays
				ListNode* listNode = mergedNodes[oldPos]->conns->first;
				while (listNode != NULL)
				{
					((MergedNode*)listNode->val)->connectedWith[oldPos] = true;
					listNode = listNode->next;
				}
			}
			array->len--;

			mergeId++;
		}
	}

	// check if number of connections (cuts) are equal
	if (mergedNodes[0]->conns->size != mergedNodes[1]->conns->size) { error(ERROR_BUG, NULL, "karger_cut.c", __LINE__); }

	// create pair of merged nodes
	MergedPair pair;
	pair.a = mergedNodes[0];
	pair.b = mergedNodes[1];
	pair.toCut = mergedNodes[0]->conns->size;

	// remove array
	array_free(array);

	return pair;
}

Array* copyGraph(Graph* graph)
{
	// create array of merged nodes
	Array* array = array_create(sizeof(MergedNode*), true);

	Node** realNodes = graph->nodes->arr;
	int realNodeCount = graph->nodes->len;

	// init all merged nodes
	for (int i = 0; i < realNodeCount; i++)
	{
		array_add(array, initMergedNode(realNodes[i], i, realNodeCount));
	}

	// it needs to be here, because array_add can reallocate arr
	MergedNode** mergedNodes = array->arr;

	// init connections of merged nodes
	for (int i = 0; i < realNodeCount; i++)
	{
		ListNode* conns = realNodes[i]->conns->first;
		MergedNode* node = mergedNodes[i];

		while (conns != NULL)
		{
			int pos = ((Node*)conns->val)->pos;
			list_append(node->conns, mergedNodes[pos]);
			node->connectedWith[pos] = true;

			conns = conns->next;
		}
	}
	return array;
}

MergedNode* initMergedNode(Node* realNode, int pos, int nodeCount)
{
	MergedNode* node = malloc(sizeof(MergedNode));
	node->pos = pos;
	node->realNodes = list_new();
	node->conns = list_new();
	node->connectedWith = calloc(nodeCount, sizeof(bool));
	node->mergeId = -1;
	list_append(node->realNodes, realNode);
	return node;
}

void mergeNodes(MergedNode* a, MergedNode* b, int mergeId)
{
	// merge lists of real nodes
	a->realNodes = list_merge(a->realNodes, b->realNodes);

	// remove connections between these two nodes
	removeConnsTo(a, b);
	removeConnsTo(b, a);

	ListNode* listNode = b->conns->first;
	while (listNode != NULL)
	{
		MergedNode* connectedNode = listNode->val;
		if (connectedNode->mergeId != mergeId)
		{
			redirectConns(connectedNode, b, a);
			connectedNode->mergeId = mergeId;
		}

		// merging connectedWith array
		a->connectedWith[connectedNode->pos] = true;

		listNode = listNode->next;
	}

	// merge lists of connections
	a->conns = list_merge(a->conns, b->conns);

	// remove b from memory; its lists were already removed when merging
	free(b->connectedWith);
	free(b);
}

void removeConnsTo(MergedNode* fromNode, MergedNode* toNode)
{
	List* conns = fromNode->conns;
	ListNode* listNode = conns->first;
	while (listNode != NULL)
	{
		ListNode* next = listNode->next;
		if (listNode->val == toNode) { list_remove(conns, listNode); }
		listNode = next;
	}

	fromNode->connectedWith[toNode->pos] = false;
}

void redirectConns(MergedNode* node, MergedNode* redirectFrom, MergedNode* redirectTo)
{
	List* conns = node->conns;
	ListNode* listNode = conns->first;
	while (listNode != NULL)
	{
		if (listNode->val == redirectFrom) { listNode->val = redirectTo; }
		listNode = listNode->next;
	}

	node->connectedWith[redirectFrom->pos] = false;
	node->connectedWith[redirectTo->pos] = true;
}

void freeMergedNode(MergedNode* node)
{
	list_free(node->realNodes);
	list_free(node->conns);
	free(node->connectedWith);
	free(node);
}

int getIterations(int iter, int nodeCount)
{
	if (iter > 0) { return iter; }

	// 50 * (nodeCount / 10)^2
	int x = nodeCount / 10;
	int defIter = 50 * x * x;

	// max(50, defIter)
	return defIter < 50 ? 50 : defIter;
}
