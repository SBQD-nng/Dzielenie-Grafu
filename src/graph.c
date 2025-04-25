#include "graph.h"
#include <stdlib.h>
#include <stdbool.h>
#include "array.h"
#include "list.h"
#include "errors.h"


// takes Node** as arguments; compares two nodes by id value
static int sortFunc(const void* a, const void* b);

// initializes graph structure
// lastSectionEnd marks end of last section - for multiple graphs it's beggining
// of the first section in next line, or -1 for single graph or last graph
// lastSectionEnd = -1 will be treated as file->conns_len
static Graph* initGraph(File* file, int arrayPos, int lastSectionEnd);

// adds connection to graph between node a and node b; order of a and b doesn't matter
static void addConn(Graph* graph, int a, int b);

// adds single connection to node; it modifies only "node" so it should be called twice
static void addConnToNode(Node* node, Node* toAdd, bool checkForDuplicate);

// adds new node to the graph
static Node* addNode(Graph* graph, int id);

// Show message and exit because of invalid graph structure
static void graphError(const char* message, int line);

// Show message that something went wrong with program
static void programError(int line);


List* graphs_init(File* file)
{
	List* graphs = list_new();

	for (int i = 0; i < file->connStarts->len; i++)
	{
		int lastSectionEnd = (i + 1 < file->connStarts->len)
				? *(int*)array_get(array_get(file->connStarts, i + 1), 0)
				: -1;
		list_append(graphs, initGraph(file, i, lastSectionEnd));
	}

	return graphs;
}

void graphs_saveConns(List* graphs, File* file)
{
	array_clear(file->conns);
	for (int i = 0; i < file->connStarts->len; i++)
	{
		array_free(array_get(file->connStarts, i));
	}
	array_clear(file->connStarts);

	ListNode* listNode = graphs->first;
	while (listNode != NULL)
	{
		Graph* graph = listNode->val;
		if (graph->nodes->len == 0) { continue; }

		Array* connStart = array_create(sizeof(int), false);
		array_add(file->connStarts, connStart);

		qsort(graph->nodes->arr, graph->nodes->len, sizeof(Node*), &sortFunc);
		for (int j = 0; j < graph->nodes->len; j++)
		{
			Node* node = array_get(graph->nodes, j);
			int id = node->id;

			bool firstAdded = false;
			ListNode* conns = node->conns->first;
			while (conns != NULL)
			{
				Node* node2 = conns->val;
				if (node2->id < id)
				{
					conns = conns->next;
					continue;
				}
				if (node2->id == id) { programError(__LINE__); }

				if (!firstAdded)
				{
					array_add(connStart, &file->conns->len);
					array_add(file->conns, &id);
					firstAdded = true;
				}

				array_add(file->conns, &node2->id);
				conns = conns->next;
			}
		}

		listNode = listNode->next;
	}
}

int sortFunc(const void* a, const void* b)
{
	return (*(Node**)a)->id - (*(Node**)b)->id;
}

Graph* initGraph(File* file, int arrayPos, int lastSectionEnd)
{
	Graph* graph = malloc(sizeof(Graph));
	graph->nodes = array_create(sizeof(Node*), true);

	int* connStart = ((Array*)array_get(file->connStarts, arrayPos))->arr;
	int connStartLen = ((Array*)array_get(file->connStarts, arrayPos))->len;
	int* conns = file->conns->arr;
	int connsLen = file->conns->len;

	if (lastSectionEnd == -1) { lastSectionEnd = connsLen; }

	for (int i = 0; i < connStartLen; i++)
	{
		int start = connStart[i];
		int end = (i + 1 < connStartLen) ? connStart[i + 1] : lastSectionEnd;

		if (start == end || start + 1 == end) { graphError("Pusta sekcja połączeń!", __LINE__); }
		if (start > end || end > connsLen || start < 0) { graphError("Błędna konfiguracja sekcji połączeń!", __LINE__); }

		int startVal = conns[start];

		for (int j = start + 1; j < end; j++)
		{
			addConn(graph, startVal, conns[j]);
		}
	}

	return graph;
}

void addConn(Graph* graph, int a, int b)
{
	if (a == b) { graphError("Próba połączenia wierzchołka z samym sobą!", __LINE__); }

	if (a > b)
	{
		int temp = a;
		a = b;
		b = temp;
	}

	Node* na = NULL;
	Node* nb = NULL;

	Node** nodes = graph->nodes->arr;
	int nodeCount = graph->nodes->len;

	// searching from the end, as node we try to find is more likely to be recently added
	for (int i = nodeCount - 1; i >= 0; i--)
	{
		if (nodes[i]->id == a)
		{
			na = nodes[i];
			break;
		}
	}
	for (int i = nodeCount - 1; i >= 0; i--)
	{
		if (nodes[i]->id == b)
		{
			nb = nodes[i];
			break;
		}
	}

	if (na == NULL) { na = addNode(graph, a); }
	if (nb == NULL) { nb = addNode(graph, b); }

	addConnToNode(na, nb, true);
	addConnToNode(nb, na, false);
}

void addConnToNode(Node* node, Node* toAdd, bool checkForDuplicate)
{
	if (checkForDuplicate)
	{
		ListNode* conns = node->conns->first;
		while (conns != NULL)
		{
			if (conns->val == toAdd) { graphError("Próba ponownego połączenia wierzchołków!", __LINE__); }
			conns = conns->next;
		}
	}
	list_append(node->conns, toAdd);
}

Node* addNode(Graph* graph, int id)
{
	Node* node = calloc(1, sizeof(Node));
	node->id = id;
	node->pos = graph->nodes->len;
	node->conns = list_new();
	array_add(graph->nodes, node);
	return node;
}

void graphError(const char* message, int line)
{
	error("Błąd przetwarzania grafu", message, "graph.c", line);
}

void programError(int line)
{
	error(ERROR_BUG, NULL, "graph.c", line);
}
