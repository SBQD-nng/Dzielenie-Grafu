#ifndef _RAYLEIGH_H_
#define _RAYLEIGH_H_

#include "../list.h"

typedef struct
{
	double val; // eigenvalue
	double* vec; // eigenvector
} Eigenpair;


// returns eigenpair with second smallest eigenvalue for given matrix
extern Eigenpair rayleigh_findEigenpairs(double* matrix, int n);

#endif
