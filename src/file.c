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

// saves array to file in text format
static void saveTextArray(FILE* stream, int* array, int arrayLen);

// saves array to file in binary format
static void saveBinArray(FILE* stream, int* array, int arrayLen);

// write int in little endian order to output stream
static void writeInt(FILE* stream, int val);

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

void file_save(File* file, char* name, int successes, bool binMode)
{
	FILE* stream = name != NULL ? fopen(name, binMode ? "wb" : "w") : stdout;
	if (stream == NULL)
	{
		fprintf(stderr, "Błąd otwierania pliku wyjściowego\n");
		exit(-1);
	}

	void (*saveArray)(FILE*, int*, int) = binMode ? saveBinArray : saveTextArray;

	if (!binMode)
	{
		fprintf(stream, "%d\n", successes);
	}

	int maxNodes[1] = {file->maxNodes};
	saveArray(stream, maxNodes, 1);
	saveArray(stream, file->xCoords, file->xCoords_len);
	saveArray(stream, file->xCoordsStart, file->xCoordsStart_len);
	saveArray(stream, file->conns, file->conns_len);

	for (int i = 0; i < file->connStarts_arrayCount; i++)
	{
		saveArray(stream, file->connStarts[i], file->connStarts_lens[i]);
	}
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

void saveTextArray(FILE* stream, int* array, int arrayLen)
{
	for (int i = 0; i < arrayLen; i++)
	{
		fprintf(stream, "%d;", array[i]);
	}
	fprintf(stream, "\n");
}

void saveBinArray(FILE* stream, int* array, int arrayLen)
{
	for (int i = 0; i < arrayLen; i++)
	{
		writeInt(stream, array[i]);
	}
	writeInt(stream, 0xFFFFFFFF);
}

void writeInt(FILE* stream, int val)
{
	char bytes[4] = {
		val & 0xFF,
		(val >> 8) & 0xFF,
		(val >> 16) & 0xFF,
		(val >> 24) & 0xFF,
	};
	fwrite(bytes, 1, 4, stream);
}

void formatError()
{
	fprintf(stderr, "Błąd formatu pliku wejściowego\n");
	exit(-1);
}
