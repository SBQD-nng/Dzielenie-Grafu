#include "array.h"
#include <stdlib.h>
#include <string.h>

#define INITIAL_SIZE 16

Array* array_create(int elementSize, bool ofPointers)
{
	Array* arr = malloc(sizeof(Array));

	arr->arr = malloc(INITIAL_SIZE * elementSize);
	arr->len = 0;
	arr->_elementSize = elementSize;
	arr->_fullSize = INITIAL_SIZE;
	arr->_ofPointers = ofPointers;

	return arr;
}

void array_add(Array* arr, void* element)
{
	if (arr->len + 1 > arr->_fullSize)
	{
		arr->_fullSize *= 2;
		arr->arr = realloc(arr->arr, arr->_fullSize * arr->_elementSize);
	}

	if (arr->_ofPointers)
	{
		((void**)arr->arr)[arr->len] = element;
	}
	else
	{
		void* pos = ((char*)arr->arr) + (arr->len * arr->_elementSize);
		memcpy(pos, element, arr->_elementSize);
	}
	arr->len++;
}

void* array_get(Array* arr, int pos)
{
	return arr->_ofPointers
			? ((void**)arr->arr)[pos]
			: ((char*)arr->arr) + (pos * arr->_elementSize);
}

void array_clear(Array* arr)
{
	free(arr->arr);
	arr->arr = malloc(INITIAL_SIZE * arr->_elementSize);
	arr->len = 0;
	arr->_fullSize = INITIAL_SIZE;
}

void array_free(Array* arr)
{
	free(arr->arr);
	free(arr);
}
