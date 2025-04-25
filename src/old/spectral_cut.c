#include "spectral_cut.h"
#include <stdio.h>
#include <stdlib.h>
#include "rayleigh.h"


// creates laplacian matrix
static double* createLaplacian(Graph* graph);

// assigns ids for each connected node, so that they will start be in range [0;n)
// where n is number of nodes in graph which part of is given node
static int assignIds(Node* node, int id);

// calculates number of removed connections between nodes for given sideArray
// side argument should be given node side
static int findCutCount(Node* node, bool* sideArray, bool side);


Cut* findSpectralCut(ListNode* listNode, double maxDiff)
{
	// create laplacian matrix
	Graph* graph = listNode->val;
	double* mat = createLaplacian(graph);
	Node** nodes = graph->nodes->arr;
	int nodeCount = graph->nodes->len;

	// if graph consists only of one node, there isn't much to cut
	if (nodeCount < 2) { return NULL; }

	// find second smallest eigenpair
	Eigenpair pair = rayleigh_findEigenpairs(mat, nodeCount);

	Cut* cut = malloc(sizeof(Cut));
	cut->graph = listNode;

	// create side array for cut, count nodes on each side and reset node marks
	cut->sideArray = malloc(nodeCount * sizeof(bool));
	int firstPartSize = 0, secondPartSize = 0;
	for (int i = 0; i < nodeCount; i++)
	{
		bool secondSide = (pair.vec[i] >= 0.0);
		cut->sideArray[i] = secondSide;

		if (secondSide) { secondPartSize ++; }
		else { firstPartSize++; }

		nodes[i]->mark = false; // reset marks, used later by findCutCount
	}

	// return NULL if two parts differ too much in size
	if (!canBeCut(firstPartSize, secondPartSize, maxDiff)) { return NULL; }

	// calculate number of removed connections in this cut
	cut->cuts = findCutCount(nodes[0], cut->sideArray, cut->sideArray[0]);

	free(pair.vec);
	free(mat);
	return cut;
}


double* createLaplacian(Graph* graph)
{
	// set all spectralCut_id values to -1
	int nodeCount = graph->nodes->len;
	Node** nodes = (Node**)graph->nodes->arr;
	for (int i = 0; i < nodeCount; i++)
	{
		nodes[i]->spectralCut_id = -1;
	}

	// recursively assign ids to all nodes in graph
	// unlike normal ids, these will always start at 0 and and at (n-1)
	int nodeCountCheck = assignIds(nodes[0], 0);

	if (nodeCount != nodeCountCheck)
	{
		fprintf(stderr, "Błąd programu! [%s:%d]\n", "matrix.c", __LINE__);
		exit(2);
	}

	// allocate matrix and init it to 0
	double* matrix = calloc(nodeCount * nodeCount, sizeof(double));

	// add degree matrix
	for (int i = 0; i < nodeCount; i++)
	{
		int pos = nodes[i]->spectralCut_id;
		matrix[(pos * nodeCount) + pos] = nodes[i]->conns->size;
	}

	// subtract adjency matrix
	for (int i = 0; i < nodeCount; i++)
	{
		Node* node1 = nodes[i];
		ListNode* conns = node1->conns->first;

		while (conns != NULL)
		{
			Node* node2 = (Node*)conns->val;
			matrix[(node1->spectralCut_id * nodeCount) + node2->spectralCut_id] = -1;
			conns = conns->next;
		}
	}
	return matrix;
}

int assignIds(Node* node, int id)
{
	node->spectralCut_id = id;
	id++;

	ListNode* conns = node->conns->first;
	while (conns != NULL)
	{
		Node* node2 = (Node*)conns->val;

		// recurse if it doesn't have an id
		if (node2->spectralCut_id == -1)
		{
			id = assignIds(node2, id);
		}

		conns = conns->next;
	}
	return id;
}

int findCutCount(Node* node, bool* sideArray, bool side)
{
	int count = 0;
	node->mark = true;

	ListNode* conns = node->conns->first;
	while (conns != NULL)
	{
		Node* node2 = (Node*)conns->val;

		if (sideArray[node2->pos] == side)
		{
			// recurse if it's in the same part and wasn't yet marked
			if (!node2->mark) { count += findCutCount(node2, sideArray, side); }
		}
		else
		{
			// increment counter if it's on the other side
			count++;
		}

		conns = conns->next;
	}
	return count;
}
