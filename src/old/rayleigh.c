#include "rayleigh.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//#include <lapacke.h>


// returns n size array of eigenpairs sorted by eigenvalue
static Eigenpair* findPairs(double* matrix, int n);

// returns single eigenvalue and modifies vec to be eigenvector for give matrix
static double rayleighIter(double* matrix, int n, double* vec, double* tempMat, double* vecY, double* tempVec);

// performs calculation [out = A - (mu * I)] where A and out is n by n matrix and I is identity matrix
static void matMinusDiag(double* a, int n, double mu, double* out);

// performs calculation [out = y - (lambda * x)] where x, y and out is vector of n elements
static void vecMinusMulVec(double* y, int n, double lambda, double* x, double* out);

// solves linear system Ax=B, where A is mat, out is x and B is vec
// returns 0 on success or non-0 when there are zeros on diagonal
static int solveLinearSystem(double* mat, int n, double* vec, double* out);

// deflate matrix
static void deflate(double* matrix, int n, double val, double* vec);

// orthogonalize matrix
static void orthogonalize(double* vec1, int n, Eigenpair* pairs, int vecCount);

// calculates dot product of two vectors
static double dotProduct(double* v1, double* v2, int n);

// calculates vector length
static double vecLen(double* vec, int n);

// normalizes vector - divides each vector element by vector length
static void normalize(double* vec, int n, double* out);

// swap a and b
static void swap(double* a, double* b);


Eigenpair rayleigh_findEigenpairs(double* matrix, int n)
{
	if (n < 2)
	{
		fprintf(stderr, "Błąd! rayleigh_findEigenpairs: n < 2\n");
		exit(1);
	}

	//====

	/*double *eigenvalues = (double *)malloc(n * sizeof(double));
	double *eigenvectors = (double *)malloc(n * n * sizeof(double));

	// Compute eigenvalues and eigenvectors
	printf("aaa\n");
	int info = LAPACKE_dsyev(LAPACK_ROW_MAJOR, 'V', 'U', n, matrix, n, eigenvalues);
	printf("bbb\n");

	if (info != 0) {
		printf("Error computing eigenvalues and eigenvectors: %d\n", info);
		free(eigenvalues);
		free(eigenvectors);
		exit(-1);
	}

	for (int i = 0; i < n; i++)
	{
		printf("value: %f\n", eigenvalues[i]); // Corresponding eigenvector
	}

	for (int i = 0; i < n; i++)
	{
		printf("ev: %f\n", matrix[(i * n) + 1]); // Corresponding eigenvector
	}

	Eigenpair pair;
	pair.val = eigenvalues[1];
	pair.vec = malloc(n * sizeof(double));

	for (int i = 0; i < n; i++)
	{
		pair.vec[i] = matrix[(i * n) + 1];
	}

	return pair;*/

	//====

	Eigenpair* pairs = findPairs(matrix, n);
	Eigenpair pair2 = pairs[1]; // second eigenpair

	for (int i = 0; i < n; i++)
	{
		if (i != 1) { free(pairs[i].vec); }
	}
	free(pairs);

	return pair2;
}

Eigenpair* findPairs(double* matrix, int n)
{
	Eigenpair* pairs = malloc(n * sizeof(Eigenpair));
	double* vec = malloc(n * sizeof(double));

	double* tempMat = malloc(n * n * sizeof(double));
	double* vecY = malloc(n * sizeof(double));
	double* tempVec = malloc(n * sizeof(double));

	for (int i = 0; i < n; i++)
	{
		for (int i = 0; i < n; i++)
		{
			vec[i] = rand() / (double)RAND_MAX;
		}

		// when i=0 it just normalizes vec
		orthogonalize(vec, n, pairs, i);

		pairs[i].val = rayleighIter(matrix, n, vec, tempMat, vecY, tempVec);
		pairs[i].vec = malloc(n * sizeof(double));
		memcpy(pairs[i].vec, vec, n * sizeof(double));

		deflate(matrix, n, pairs[i].val, vec);
	}

	free(tempVec);
	free(vecY);
	free(tempMat);
	return pairs;
}

double rayleighIter(double* matrix, int n, double* vec, double* tempMat, double* vecY, double* tempVec)
{
	memcpy(vecY, vec, n * sizeof(double));
	double mu = 200.0, err = 1.0, lambda;

	while (err > 1e-12)
	{
		normalize(vecY, n, vec);

		matMinusDiag(matrix, n, mu, tempMat);
		if (solveLinearSystem(tempMat, n, vec, vecY) != 0) { break; }

		lambda = dotProduct(vecY, vec, n);
		mu = mu + 1.0 / lambda;

		vecMinusMulVec(vecY, n, lambda, vec, tempVec);
		err = vecLen(tempVec, n) / vecLen(vecY, n);
	}
	return mu;
}

void matMinusDiag(double* a, int n, double mu, double* out)
{
	// out = A - (mu * I)
	memcpy(out, a, n * n * sizeof(double));
	for (int i = 0; i < n; i++)
	{
		out[(i * n) + i] -= mu;
	}
}

void vecMinusMulVec(double* y, int n, double lambda, double* x, double* out)
{
	// out = y - (lambda * x)
	memcpy(out, y, n*sizeof(double));
	for (int i = 0; i < n; i++)
	{
		out[i] -= lambda * x[i];
	}
}

int solveLinearSystem(double* mat, int n, double* vec, double* out)
{
	const double EPS = 1e-12;
	memcpy(out, vec, n * sizeof(double));

	for (int i = 0; i < n; i++)
	{
		// select main element
		double maxVal = fabs(mat[(i * n) + i]);
		int pos = i;
		for (int j = i + 1; j < n; j++)
		{
			double val = fabs(mat[(j * n) + i]);
			if (val > maxVal)
			{
				maxVal = val;
				pos = j;
			}
		}

		// swap rows
		if (pos != i)
		{
			for(int j = 0; j < n; j++)
			{
				swap(&mat[(i * n) + j], &mat[(pos * n) + j]);
			}
			swap(&out[i], &out[pos]);
		}

		// check if there are zeros on diagonal
		if (fabs(mat[(i * n) + i]) < EPS) { return -1; }

		// set all rows below 0 in this column
		for (int j = i + 1; j < n; j++)
		{
			double factor = mat[(j * n) + i] / mat[(i * n) + i];
			for(int k = i; k < n; k++)
			{
				mat[(j * n) + k] -= mat[(i * n) + k] * factor;
			}
			out[j] -= out[i] * factor;
		}
	}

	// back substitution
	for (int i = n - 1; i >= 0; i--)
	{
		out[i] /= mat[(i * n) + i];
		for (int j = i - 1; j >= 0; j--)
		{
			out[j] -= mat[(j * n) + i] * out[i];
		}
	}
	return 0;
}

void deflate(double* matrix, int n, double val, double* vec)
{
	normalize(vec, n, vec);

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			matrix[(i * n) + j] -= val * vec[i] * vec[j];
		}
	}
}

void orthogonalize(double* vec1, int n, Eigenpair* pairs, int vecCount)
{
	for (int i = 0; i < vecCount; i++)
	{
		double* vec2 = pairs[i].vec;
		double dotProd = dotProduct(vec1, vec2, n);
		for (int j = 0; j < n; j++)
		{
			vec1[j] -= vec2[j] * dotProd;
		}
	}
	normalize(vec1, n, vec1);
}

double dotProduct(double* v1, double* v2, int n)
{
	double val = 0.0;
	for (int i = 0; i < n; i++)
	{
		val += v1[i] * v2[i];
	}
	return val;
}

double vecLen(double* vec, int n)
{
	double val = 0.0;
	for (int i = 0; i < n; i++)
	{
		val += vec[i] * vec[i];
	}
	return sqrt(val);
}

void normalize(double* vec, int n, double* out)
{
	double len = vecLen(vec, n);
	for (int i = 0; i < n; i++)
	{
		out[i] = vec[i] / len;
	}
}

void swap(double* a, double* b)
{
	double temp = *a;
	*a = *b;
	*b = temp;
}
