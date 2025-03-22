#ifndef _FILE_H_
#define _FILE_H_

#include <stdbool.h>
#include "array.h"

typedef struct
{
	int maxNodes;

	Array* xCoords; // of int
	Array* xCoordsStart; // of int

	Array* conns; // of int
	Array* connStarts; // of Array* of int
} File;


// loads file in text format
extern File* file_load(const char* name);

// saves file in text or binary mode (with text mode starting with line containing number of successes)
// if name == NULL it's saved to stdout
extern void file_save(File* file, const char* name, int successes, bool binMode);

#endif
