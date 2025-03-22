#include <stdio.h>
#include "arguments.h"
#include "file.h"
#include "graph.h"

int main(int argc, char** argv)
{
	Arguments* args = arguments_parse(argc, argv);
	if (args->inputFile == NULL)
	{
		fprintf(stderr, "Nie określono pliku wejściowego\n");
		return -1;
	}

	File* file = file_load(args->inputFile);
	file_save(file, args->outputFile, 123, args->useBinaryMode);

	Graphs* graphs = graphs_init(file);
	graphs_saveConns(graphs, file);
	file_save(file, "graf_przetworzony.out", 999, args->useBinaryMode);
	return 0;
}
