#include "arguments.h"
#include "file.h"
#include "graph.h"
//#include "simple_cut.h"
#include "cut.h"
#include "karger_cut.h"
#include "errors.h"

#include <stdlib.h>
#include <time.h>

int main(int argc, char** argv)
{
	srand(time(NULL)); //TODO: move

	Arguments* args = arguments_parse(argc, argv);
	if (args->inputFile == NULL) { error("Nie określono pliku wejściowego!", NULL, "main.c", __LINE__); }

	File* file = file_load(args->inputFile);

	List* graphs = graphs_init(file);
	int successes = cutGraphs(&graphs, args->divisions, args->maxDiff, &findKargerCut);
	//int successes = cutGraphs(&graphs, args->divisions, args->maxDiff, &findSimpleCut);

	graphs_saveConns(graphs, file);
	file_save(file, args->outputFile, successes, args->useBinaryMode);
	return 0;
}
