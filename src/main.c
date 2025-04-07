#include <stdio.h>
#include "arguments.h"
#include "file.h"
#include "graph.h"
#include "simpleCut.h"

int main(int argc, char** argv)
{
	Arguments* args = arguments_parse(argc, argv);
	if (args->inputFile == NULL)
	{
		fprintf(stderr, "Nie określono pliku wejściowego\n");
		return -1;
	}

	File* file = file_load(args->inputFile);

	Array* graphs = graphs_init(file);
	int successes = simpleCutGraphs(graphs, args->divisions, args->maxDiff);

	graphs_saveConns(graphs, file);
	file_save(file, args->outputFile, successes, args->useBinaryMode);
	return 0;
}
