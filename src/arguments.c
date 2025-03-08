#include "arguments.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


static const char* DEFAULT_OUTPUT = "graf.out";


// Parses argument that isn't flag - doesn't start with '-'
static void parseArg(Arguments* args, char** argv, int i, int* argNum);

// Parses flag - argv[i][0] should equal '-'
static void parseFlag(Arguments* args, int argc, char** argv, int* i);

// Show message and exit because of invalid user input
static void argError(const char* message);


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
			//TODO: handle invalid strings?
			if (args->divisions < 0) { argError("Podana liczba podziałów jest ujemna!"); }
			break;

		case 2:
			args->maxDiff = strtod(argv[i], NULL);
			if (args->maxDiff < 0.0) { argError("Podana różnica procentowa jest ujemna!"); }
			break;

		default:
			argError("Podano za dużo argumentów!");
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
				argError("Podano flagę -o bez podania nazwy pliku wyjściowego!");
			}
			else if (args->outputFile != DEFAULT_OUTPUT)
			{
				if (args->outputFile != NULL) { argError("Próba użycia -o dwukrotnie!"); }
				else { argError("Próba użycia -o i -t jednocześnie!"); }
			}

			args->outputFile = argv[(*i) + 1];
			(*i)++;
			break;

		case 't':
			if (args->outputFile != DEFAULT_OUTPUT)
			{
				if (args->outputFile == NULL) { argError("Próba użycia -t dwukrotnie!"); }
				else { argError("Próba użycia -o i -t jednocześnie!"); }
			}

			args->outputFile = NULL;
			break;

		case 'b':
			if (args->useBinaryMode) { argError("Próba użycia -b dwukrotnie!"); }
			args->useBinaryMode = true;
			break;

		default:
			argError("Próba użycia nieznanej flagi!");
			//TODO: wypisanie flagi
	}
}

void argError(const char* message)
{
	//TODO: wypisywanie linijki i pliku
	fprintf(stderr, "Błąd wczytywania argumentów: %s\n", message);
	exit(1);
}
