#ifndef _ARRAY_H_
#define _ARRAY_H_

#include <stdbool.h>

typedef struct
{
	void* arr;
	int len;
	int _elementSize;
	int _fullSize;
	bool _ofPointers;
} Array;


// creates dynamic array
extern Array* array_create(int elementSize, bool ofPointers);

// adds element to array
extern void array_add(Array* arr, void* element);

// retrieves element from array
extern void* array_get(Array* arr, int pos);

// clear array without removing it
extern void array_clear(Array* arr);

// removes array
extern void array_free(Array* arr);

#endif
