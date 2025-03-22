#ifndef _GRAPH_H_
#define _GRAPH_H_

#include "array.h"
#include "file.h"

typedef struct
{
	int id;
	Array* conns; // of Node*
} Node;

typedef struct
{
	Array* nodes; // of Node*
} Graph;


// initializes graphs - Array of Graph*
extern Array* graphs_init(File* file);

// saves connections from Array of Graph* to File structure
extern void graphs_saveConns(Array* graphs, File* file);

#endif
