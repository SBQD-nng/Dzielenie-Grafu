#include "file.h"
#include "array.h"
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


// reads line to Array of ints
// when array is empty it throws error or returns NULL, depending on canBeEmpty argument
static Array* readArray(FILE* stream, bool canBeEmpty);

// reads char from with BufferedReader
static char readChar(BufferedReader* reader);

// saves array to file in text format
static void saveTextArray(FILE* stream, Array* array);

// saves array to file in binary format
static void saveBinArray(FILE* stream, Array* array);

// write int in little endian order to output stream
static void writeInt(FILE* stream, int val);

// prints format error message and exits
static void formatError(int line);


File* file_load(const char* name)
{
	FILE* stream = fopen(name, "r");
	if (stream == NULL)
	{
		fprintf(stderr, "Błąd otwierania pliku wejściowego\n");
		exit(-1);
	}

	File* file = malloc(sizeof(File));

	Array* line = readArray(stream, false);
	if (line->len != 1) { formatError(__LINE__); }
	file->maxNodes = *(int*)array_get(line, 0);
	array_free(line);

	file->xCoords = readArray(stream, false);
	file->xCoordsStart = readArray(stream, false);
	file->conns = readArray(stream, false);

	file->connStarts = array_create(sizeof(Array*), true);
	for (;;)
	{
		line = readArray(stream, true);
		if (line == NULL) { break; }

		array_add(file->connStarts, line);
	}
	if (file->connStarts->len == 0) { formatError(__LINE__); }

	return file;
}

void file_save(File* file, const char* name, int successes, bool binMode)
{
	FILE* stream = name != NULL ? fopen(name, binMode ? "wb" : "w") : stdout;
	if (stream == NULL)
	{
		fprintf(stderr, "Błąd otwierania pliku wyjściowego\n");
		exit(-1);
	}

	void (*saveArray)(FILE*, Array*) = binMode ? saveBinArray : saveTextArray;

	if (!binMode)
	{
		fprintf(stream, "%d\n", successes);
	}

	Array* maxNodes = array_create(sizeof(int), false);
	array_add(maxNodes, &file->maxNodes);
	saveArray(stream, maxNodes);
	array_free(maxNodes);

	saveArray(stream, file->xCoords);
	saveArray(stream, file->xCoordsStart);
	saveArray(stream, file->conns);

	for (int i = 0; i < file->connStarts->len; i++)
	{
		saveArray(stream, array_get(file->connStarts, i));
	}
}

Array* readArray(FILE* stream, bool canBeEmpty)
{
	char buf[BUF_SIZE];
	BufferedReader reader;
	reader.stream = stream;
	reader.buf = buf;
	reader.bufPtr = -1;

	Array* arr = array_create(sizeof(int), false);

	char ch;
	long long num = -1;
	while (1)
	{
		ch = readChar(&reader);

		if (ch >= '0' && ch <= '9')
		{
			if (num == -1) { num = 0; }

			num *= 10;
			num += (int)(ch - '0');

			if (num != (int)num) { formatError(__LINE__); }
		}
		else if (ch == ';' || ch == '\n')
		{
			if (num != -1)
			{
				int intNum = (int)num;
				array_add(arr, &intNum);
				num = -1;
			}

			if (ch == '\n') { break; }
		}
		else
		{
			formatError(__LINE__);
		}
	}

	if (arr->len == 0)
	{
		array_free(arr);

		if (canBeEmpty) { return NULL; }
		else { formatError(__LINE__); }
	}

	return arr;
}

char readChar(BufferedReader* reader)
{
	if (reader->bufPtr == -1 || reader->buf[reader->bufPtr + 1] == '\0')
	{
		char* out = fgets(reader->buf, BUF_SIZE, reader->stream);
		reader->bufPtr = -1;

		if (out == NULL) { return '\n'; }
	}

	reader->bufPtr++;
	return reader->buf[reader->bufPtr];
}

void saveTextArray(FILE* stream, Array* array)
{
	for (int i = 0; i < array->len; i++)
	{
		fprintf(stream, "%d;", *(int*)array_get(array, i));
	}
	fprintf(stream, "\n");
}

void saveBinArray(FILE* stream, Array* array)
{
	for (int i = 0; i < array->len; i++)
	{
		writeInt(stream, *(int*)array_get(array, i));
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

void formatError(int line)
{
	fprintf(stderr, "Błąd formatu pliku wejściowego! [file.c:%d]\n", line);
	exit(-1);
}
