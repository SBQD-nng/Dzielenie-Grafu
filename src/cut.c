#include "cut.h"
#include <stdlib.h>
#include <math.h>
#include "array.h"
#include "graph.h"
#include "list.h"


// performs cut on graph - returns array of graphs after cutting
static List* performCut(Cut* cut);

// creates list of graphs from old list of nodes that were splitted
static List* createGraphs(Node** nodes, int nodeCount);

// recursively adds connected nodes to graph array
static void addNodesToGraph(Node* node, Array* array);

// adds cut to sorted list of cuts
static void addCutToList(List* list, Cut* cut);


int cutGraphs(List** graphs, int n, double maxDiff, Cut* (*findCut)(ListNode*, double, int), int specialArg)
{
	if (n < 1) { return 0; }
	maxDiff /= 100.0;

	List* cutList = list_new();
	ListNode* graphsNode = (*graphs)->first;
	while (graphsNode != NULL)
	{
		Cut* cut = findCut(graphsNode, maxDiff, specialArg);
		if (cut != NULL) { addCutToList(cutList, cut); }
		graphsNode = graphsNode->next;
	}

	for (int i = 0;; i++)
	{
		if (cutList->first == NULL) { return i; } // return i if there aren't any valid cuts available

		// perform simple cut and add new graph to graphs
		Cut* cut = (Cut*)cutList->first->val;
		List* newGraphs = performCut(cut);

		// remove old graph from old list
		list_remove(*graphs, cut->graph);

		// merge old and new list, but first save first node of new graphs
		ListNode* firstNewGraph = newGraphs->first;
		*graphs = list_merge(*graphs, newGraphs);

		// free cut and remove it from cutList
		free(cut->sideArray);
		free(cut);
		list_remove(cutList, cutList->first);

		// exit if it performed n cuts
		if (i == n - 1) { break; }

		// check new graphs for cuts and add to cutList if valid
		while (firstNewGraph != NULL)
		{
			Cut* newCut = findCut(firstNewGraph, maxDiff, specialArg);
			if (newCut != NULL) { addCutToList(cutList, newCut); }
			firstNewGraph = firstNewGraph->next;
		}
	}
	return n;
}

bool canBeCut(int firstPartSize, int secondPartSize, double maxDiff)
{
	if (firstPartSize == 0 || secondPartSize == 0) return false;
	double a = firstPartSize, b = secondPartSize;

	return fabs((a - b) / ((a + b) / 2.0)) <= maxDiff;
}

List* performCut(Cut* cut)
{
	Graph* graph = cut->graph->val;
	int nodeCount = graph->nodes->len;
	Node** nodes = (Node**)graph->nodes->arr;

	for (int i = 0; i < nodeCount; i++)
	{
		bool secondPart = cut->sideArray[i];

		ListNode* conns = nodes[i]->conns->first;
		while (conns != NULL)
		{
			ListNode* nextConn = conns->next; // preventing read after free
			if (cut->sideArray[((Node*)conns->val)->pos] != secondPart)
			{
				list_remove(nodes[i]->conns, conns);
			}
			conns = nextConn;
		}
	}

	// create new graphs and remove old graph
	List* graphs = createGraphs(nodes, nodeCount);
	array_free(graph->nodes);
	free(graph);

	return graphs;
}

List* createGraphs(Node** nodes, int nodeCount)
{
	List* graphs = list_new();

	// reset all marks
	for (int i = 0; i < nodeCount; i++)
	{
		nodes[i]->mark = false;
	}

	for (int i = 0; i < nodeCount; i++)
	{
		if (nodes[i]->mark) { continue; } // skip if marked

		Graph* graph = malloc(sizeof(Graph));
		graph->nodes = array_create(sizeof(Node*), true);
		addNodesToGraph(nodes[i], graph->nodes);

		list_append(graphs, graph);
	}
	return graphs;
}

void addNodesToGraph(Node* node, Array* array)
{
	node->pos = array->len;
	node->mark = true;
	array_add(array, node);

	ListNode* conns = node->conns->first;
	while (conns != NULL)
	{
		Node* node2 = (Node*)conns->val;
		if (!node2->mark) { addNodesToGraph(node2, array); }
		conns = conns->next;
	}
}

void addCutToList(List* list, Cut* cut)
{
	ListNode* node = list->first;
	while (node != NULL)
	{
		if (((Cut*)node->val)->cuts > cut->cuts)
		{
			list_insert(list, node->prev, cut);
			return;
		}
		node = node->next;
	}

	// if it wasn't inserted yet it means cut->cuts is higher then all other cuts
	// so append it on a tail of list
	list_append(list, cut);
}
