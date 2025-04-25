#include "errors.h"
#include <stdio.h>
#include <stdlib.h>

void error(const char* message1, const char* message2, const char* file, int line)
{
	if (message2 == NULL) { fprintf(stderr, "%s [%s:%d]\n", message1, file, line); }
	else { fprintf(stderr, "%s: %s [%s:%d]\n", message1, message2, file, line); }
	exit(-1);
}
