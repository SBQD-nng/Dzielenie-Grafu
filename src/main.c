#include <stdio.h>
#include "arguments.h"
#include "file.h"

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
	return 0;
}
