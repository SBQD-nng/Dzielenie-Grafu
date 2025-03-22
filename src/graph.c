#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


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
static void graphError(const char* message);

// Show message that something went wrong with program
static void programError(const char* module, int line);


Graphs* graphs_init(File* file)
{
	int count = file->connStarts_arrayCount;

	Graphs* graphs = malloc(sizeof(Graphs));
	graphs->graphs = malloc(count * sizeof(Graph*));
	graphs->count = count;

	for (int i = 0; i < file->connStarts_arrayCount; i++)
	{
		int lastSectionEnd = (i + 1 < file->connStarts_arrayCount) ? file->connStarts[i + 1][0] : -1;
		graphs->graphs[i] = initGraph(file, i, lastSectionEnd);
	}

	return graphs;
}

static int sortFunc(const void* a, const void* b) //TODO: remove
{
	return (*(Node**)a)->id - (*(Node**)b)->id;
}

void graphs_saveConns(Graphs* graphs, File* file)
{
	free(file->conns);
	for (int i = 0; i < file->connStarts_arrayCount; i++)
	{
		free(file->connStarts[i]);
	}
	free(file->connStarts);
	free(file->connStarts_lens);

	file->conns = NULL;
	file->conns_len = 0;
	file->connStarts = NULL;
	file->connStarts_lens = NULL;
	file->connStarts_arrayCount = 0;

	for (int i = 0; i < graphs->count; i++)
	{
		Graph* graph = graphs->graphs[i];
		if (graph->nodeCount == 0) { continue; }

		file->connStarts_arrayCount++;
		file->connStarts = realloc(file->connStarts, file->connStarts_arrayCount * sizeof(int*));
		file->connStarts_lens = realloc(file->connStarts_lens, file->connStarts_arrayCount * sizeof(int));

		int** connStart = &file->connStarts[file->connStarts_arrayCount - 1];
		int* connStartLen = &file->connStarts_lens[file->connStarts_arrayCount - 1];
		*connStart = NULL;
		*connStartLen = 0;

		qsort(graph->nodes, graph->nodeCount, sizeof(Node*), &sortFunc); //TODO: remove
		for (int j = 0; j < graph->nodeCount; j++)
		{
			Node* node = graph->nodes[j];
			int id = node->id;

			bool firstAdded = false;
			for (int k = 0; k < node->connCount; k++)
			{
				Node* node2 = (Node*)node->conns[k];
				if (node2->id < id) { continue; }
				if (node2->id == id) { programError("graph.c", __LINE__); }

				if (!firstAdded)
				{
					*connStart = realloc(*connStart, (*connStartLen + 1) * sizeof(int));
					(*connStart)[*connStartLen] = file->conns_len;
					(*connStartLen)++;

					file->conns = realloc(file->conns, (file->conns_len + 1) * sizeof(int));
					file->conns[file->conns_len] = id;
					file->conns_len++;
					firstAdded = true;
				}

				file->conns = realloc(file->conns, (file->conns_len + 1) * sizeof(int));
				file->conns[file->conns_len] = node2->id;
				file->conns_len++;
			}
		}
	}
}

Graph* initGraph(File* file, int arrayPos, int lastSectionEnd)
{
	Graph* graph = calloc(1, sizeof(Graph));

	int* connStart = file->connStarts[arrayPos];
	int connStartLen = file->connStarts_lens[arrayPos];
	int* conns = file->conns;
	int connsLen = file->conns_len;

	if (lastSectionEnd == -1) { lastSectionEnd = connsLen; }

	for (int i = 0; i < connStartLen; i++)
	{
		int start = connStart[i];
		int end = (i + 1 < connStartLen) ? connStart[i + 1] : lastSectionEnd;

		if (start == end || start + 1 == end) { graphError("Pusta sekcja połączeń!"); }
		if (start > end || end > connsLen || start < 0) { graphError("Błędna konfiguracja sekcji połączeń!"); }

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
	if (a == b) { graphError("Próba połączenia wierzchołka z samym sobą!"); }

	if (a > b)
	{
		int temp = a;
		a = b;
		b = temp;
	}

	Node* na = NULL;
	Node* nb = NULL;

	// searching from the end, as node we try to find is more likely to be recently added
	for (int i = graph->nodeCount - 1; i >= 0; i--)
	{
		if (graph->nodes[i]->id == a)
		{
			na = graph->nodes[i];
			break;
		}
	}
	for (int i = graph->nodeCount - 1; i >= 0; i--)
	{
		if (graph->nodes[i]->id == b)
		{
			nb = graph->nodes[i];
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
		for (int i = 0; i < node->connCount; i++)
		{
			if (node->conns[i] == toAdd) { graphError("Próba ponownego połączenia wierzchołków!"); }
		}
	}

	node->conns = realloc(node->conns, (node->connCount + 1) * sizeof(Node*));
	node->conns[node->connCount] = toAdd;
	node->connCount++;
}

Node* addNode(Graph* graph, int id)
{
	Node* node = calloc(1, sizeof(Node));
	node->id = id;

	graph->nodes = realloc(graph->nodes, (graph->nodeCount + 1) * sizeof(Node*));
	graph->nodes[graph->nodeCount] = node;
	graph->nodeCount++;

	return node;
}

void graphError(const char* message)
{
	fprintf(stderr, "Błąd przetwarzania grafu: %s\n", message);
	exit(1);
}

void programError(const char* module, int line)
{
	fprintf(stderr, "Błąd programu! [%s:%d]\n", module, line);
	exit(2);
}
