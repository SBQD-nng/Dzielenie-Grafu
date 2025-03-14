#ifndef _ARGUMENTS_H_
#define _ARGUEMNTS_H_

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

// saves file to text format (with line containing number of successes)
extern void file_saveText(char* name, int successes);

// saves file to binary format
extern void file_saveBin(char* name);

#endif
