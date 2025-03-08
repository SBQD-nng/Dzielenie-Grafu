#ifndef _ARGUMENTS_H_
#define _ARGUEMNTS_H_

#include <stdbool.h>

typedef struct
{
	const char* inputFile; // NULL means it wasn't specified
	const char* outputFile; // NULL means stdout

	int divisions;
	double maxDiff; // in percentages

	bool useBinaryMode;
} Arguments;


// parses main() arguments
extern Arguments* arguments_parse(int argc, char** argv);

#endif
