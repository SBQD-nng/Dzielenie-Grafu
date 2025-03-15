#ifndef _ARGUMENTS_H_
#define _ARGUEMNTS_H_

#include <stdbool.h>

typedef struct
{
	int maxNodes;

	int* xCoords;
	int xCoords_len;

	int* xCoordsStart;
	int xCoordsStart_len;

	int* conns;
	int conns_len;

	int** connStarts;
	int* connStarts_lens;
	int connStarts_arrayCount;
} File;


// loads file in text format
extern File* file_load(char* name);

// saves file in text or binary mode (with text mode starting with line containing number of successes)
// if name == NULL it's saved to stdout
extern void file_save(File* file, char* name, int successes, bool binMode);

#endif
