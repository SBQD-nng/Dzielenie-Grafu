#ifndef _GRAPH_H_
#define _GRAPH_H_

#include "file.h"

typedef struct
{
	int id;
	void** conns;
	int connCount;
} Node;

typedef struct
{
	Node** nodes;
	int nodeCount;
} Graph;

typedef struct
{
	Graph** graphs;
	int count;
} Graphs;


// initializes graphs
extern Graphs* graphs_init(File* file);

// saves connections from Graphs to File structure
extern void graphs_saveConns(Graphs* graphs, File* file);

#endif
