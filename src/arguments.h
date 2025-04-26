#ifndef _ARGUMENTS_H_
#define _ARGUMENTS_H_

#include <stdbool.h>


typedef struct
{
	const char* inputFile; // NULL means it wasn't specified
	const char* outputFile; // NULL means stdout

	int divisions;
	double maxDiff; // in percentages

	bool useBinaryMode;

	/*
		Methods:
		0/1  - Karger's algorithm (-1 means flag wasn't used)
		2    - Simple method (2^n) with graph connectivity verifications.
			   Like with Karger's algorithm it won't split graph if two sides aren't connected graphs
		3    - Simple method (2^n) without graph connectivity verifications.
	*/
	int method;

	bool definedSeed; // works only with method = 0
	int seed;

	int iterations; // if < 1 - auto
} Arguments;


// parses main() arguments
extern Arguments* arguments_parse(int argc, char** argv);

#endif
