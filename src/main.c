#include <stdlib.h>
#include <time.h>
#include "arguments.h"
#include "file.h"
#include "graph.h"
#include "cut.h"
#include "simple_cut.h"
#include "karger_cut.h"
#include "errors.h"

int main(int argc, char** argv)
{
	Arguments* args = arguments_parse(argc, argv);
	if (args->inputFile == NULL) { error("Nie określono pliku wejściowego!", NULL, "main.c", __LINE__); }

	// set seed
	srand(args->definedSeed ? args->seed : time(NULL));

	File* file = file_load(args->inputFile);

	List* graphs = graphs_init(file);
	int successes = (args->method > 1)
			? cutGraphs(&graphs, args->divisions, args->maxDiff, &findSimpleCut, (args->method == 2))
			: cutGraphs(&graphs, args->divisions, args->maxDiff, &findKargerCut, args->iterations);

	graphs_saveConns(graphs, file);
	file_save(file, args->outputFile, successes, args->useBinaryMode);
	return 0;
}
