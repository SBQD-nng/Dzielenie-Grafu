#include "file.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define BUF_SIZE 2048

typedef struct
{
	FILE* stream;
	char* buf;
	int bufPtr;
} BufferedReader;


// reads line to int array and saves it size to outLen
// when array is empty it throws error or returns NULL, depending on canBeEmpty argument
static int* readArray(FILE* stream, int* outLen, bool canBeEmpty);

// reads char from with BufferedReader
static char readChar(BufferedReader* reader);

// prints format error message and exits
static void formatError();


File* file_load(char* name)
{
	FILE* stream = fopen(name, "r");
	if (stream == NULL)
	{
		fprintf(stderr, "Błąd otwierania pliku wejściowego\n");
		exit(-1);
	}

	File* file = malloc(sizeof(File));

	int lineLen;
	int* line;

	line = readArray(stream, &lineLen, false);
	if (lineLen != 1)
	{
		formatError();
	}
	file->maxNodes = line[0];
	free(line);

	file->xCoords = readArray(stream, &file->xCoords_len, false);
	file->xCoordsStart = readArray(stream, &file->xCoordsStart_len, false);
	file->conns = readArray(stream, &file->conns_len, false);

	file->connStarts = NULL;
	file->connStarts_lens = NULL;
	file->connStarts_arrayCount = 0;
	while (1)
	{
		line = readArray(stream, &lineLen, true);
		if (line == NULL)
		{
			break;
		}

		int arrayCount = file->connStarts_arrayCount;
		file->connStarts = realloc(file->connStarts, (arrayCount + 1) * sizeof(int*));
		file->connStarts_lens = realloc(file->connStarts, (arrayCount + 1) * sizeof(int));
		file->connStarts[arrayCount] = line;
		file->connStarts_lens[arrayCount] = lineLen;

		file->connStarts_arrayCount++;
	}
	if (file->connStarts_arrayCount == 0)
	{
		formatError();
	}

	return file;
}

void file_saveText(char* name, int successes)
{

}

void file_saveBin(char* name)
{

}

int* readArray(FILE* stream, int* outLen, bool canBeEmpty)
{
	char buf[BUF_SIZE];
	BufferedReader reader;
	reader.stream = stream;
	reader.buf = buf;
	reader.bufPtr = -1;

	int* array = malloc(16 * sizeof(int));
	int arrayAllocSize = 16;
	int arrayLen = 0;

	char ch;
	long long num = -1;
	while (1)
	{
		ch = readChar(&reader);

		if (ch >= '0' && ch <= '9')
		{
			if (num == -1)
			{
				num = 0;
			}

			num *= 10;
			num += (int)(ch - '0');

			if (num != (int)num)
			{
				formatError();
			}
		}
		else if (ch == ';' || ch == '\n')
		{
			if (num == -1)
			{
				if (arrayLen == arrayAllocSize)
				{
					arrayAllocSize *= 2;
					array = realloc(array, arrayAllocSize * sizeof(int));
				}

				array[arrayLen] = (int)num;
				arrayLen++;
				num = -1;
			}

			if (ch == '\n')
			{
				break;
			}
		}
		else
		{
			formatError();
		}
	}

	if (arrayLen == 0)
	{
		free(array);

		if (canBeEmpty)
		{
			return NULL;
		}
		else
		{
			formatError();
		}
	}

	*outLen = arrayLen;
	return array;
}

char readChar(BufferedReader* reader)
{
	if (reader->bufPtr == -1 || reader->buf[reader->bufPtr + 1] == '\0')
	{
		char* out = fgets(reader->buf, BUF_SIZE, reader->stream);
		reader->bufPtr = -1;

		if (out == NULL)
		{
			return '\n';
		}
	}

	reader->bufPtr++;
	return reader->buf[reader->bufPtr];
}

void formatError()
{
	fprintf(stderr, "Błąd formatu pliku wejściowego\n");
	exit(-1);
}
