#include "arguments.h"
#include <stdlib.h>
#include <stdbool.h>
#include "errors.h"


static const char* DEFAULT_OUTPUT = "graf.out";


// Parses argument that isn't flag - doesn't start with '-'
static void parseArg(Arguments* args, char** argv, int i, int* argNum);

// Parses flag - argv[i][0] should equal '-'
static void parseFlag(Arguments* args, int argc, char** argv, int* i);

// Show message and exit because of invalid user input
static void argError(const char* message, int line);


Arguments* arguments_parse(int argc, char** argv)
{
	// initialize array to default arguments
	Arguments* args = malloc(sizeof(Arguments));
	args->inputFile = NULL;
	args->outputFile = DEFAULT_OUTPUT;
	args->divisions = 1;
	args->maxDiff = 10;
	args->useBinaryMode = false;

	// loop over argv
	int argNum = 0;
	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-') { parseFlag(args, argc, argv, &i); }
		else { parseArg(args, argv, i, &argNum); }
	}

	return args;
}

void parseArg(Arguments* args, char** argv, int i, int* argNum)
{
	switch (*argNum)
	{
		case 0:
			args->inputFile = argv[i];
			break;

		case 1:
			args->divisions = strtoll(argv[i], NULL, 10);
			if (args->divisions < 0) { argError("Podana liczba podziałów jest ujemna!", __LINE__); }
			break;

		case 2:
			args->maxDiff = strtod(argv[i], NULL);
			if (args->maxDiff < 0.0) { argError("Podana różnica procentowa jest ujemna!", __LINE__); }
			break;

		default:
			argError("Podano za dużo argumentów!", __LINE__);
	}

	(*argNum)++;
}

void parseFlag(Arguments* args, int argc, char** argv, int* i)
{
	char flag = argv[*i][1];

	switch (flag)
	{
		case 'o':
			if ((*i) + 1 >= argc)
			{
				argError("Podano flagę -o bez podania nazwy pliku wyjściowego!", __LINE__);
			}
			else if (args->outputFile != DEFAULT_OUTPUT)
			{
				if (args->outputFile != NULL) { argError("Próba użycia -o dwukrotnie!", __LINE__); }
				else { argError("Próba użycia -o i -t jednocześnie!", __LINE__); }
			}

			args->outputFile = argv[(*i) + 1];
			(*i)++;
			break;

		case 't':
			if (args->outputFile != DEFAULT_OUTPUT)
			{
				if (args->outputFile == NULL) { argError("Próba użycia -t dwukrotnie!", __LINE__); }
				else { argError("Próba użycia -o i -t jednocześnie!", __LINE__); }
			}

			args->outputFile = NULL;
			break;

		case 'b':
			if (args->useBinaryMode) { argError("Próba użycia -b dwukrotnie!", __LINE__); }
			args->useBinaryMode = true;
			break;

		default:
			argError("Próba użycia nieznanej flagi!", __LINE__);
	}
}

void argError(const char* message, int line)
{
	error("Błąd wczytywania argumentów", message, "arguments.c", line);
}
