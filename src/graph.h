#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <stdbool.h>
#include "array.h"
#include "file.h"
#include "list.h"

typedef struct
{
	int id, pos;
	List* conns; // of Node*
	bool mark;

	bool simpleCut_secondPart;
	//int spectralCut_id;
} Node;

typedef struct
{
	Array* nodes; // of Node*
} Graph;


// initializes graphs - List of Graph*
extern List* graphs_init(File* file);

// saves connections from List of Graph* to File structure
extern void graphs_saveConns(List* graphs, File* file);

#endif
