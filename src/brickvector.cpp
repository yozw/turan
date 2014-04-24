#include <iostream>
#include <string.h>

#include "brickvector.h"
#include "lex_sort.h"
#include "permutation.h"

/* Macros to extract information from brick flags */
inline int getN(const CFINT* vector)             { return vector[0]; }
inline int getK(const CFINT* vector)             { return vector[1]; }
inline int getNlabelled(const CFINT* vector)     { return vector[2]; }
inline int getKlabelled(const CFINT* vector)     { return vector[3]; }
inline int getCrossingCount(const CFINT* vector) { return vector[4]; }
inline int getLength(const CFINT* vector)        { return 5 + getCrossingCount(vector) * 4; }

#define CROSSING_OFFSET          5

std::string topvertices    = "abcdef";
std::string bottomvertices = "wxyz";


void print_vector(const CFINT* vector)
{
	print_vector(vector, std::cout);
}

void print_vector(const CFINT* vector, std::ostream& stream)
{
	if (vector == NULL)
	{
		stream << "NULL" << std::endl;
		return;
	}

//	std::cout << "hash = " << vector_hash_value(vector) << " ";

	for (int i = 0; i < 5; i++) {
		stream << (int) vector[i]; 
		if (i < 4) stream << " ";
	}

	for (int i = 0; i < getCrossingCount(vector); i++)
	{
		stream << " ";
		stream << topvertices[vector[CROSSING_OFFSET + 4 * i]] << " ";
		stream << bottomvertices[vector[CROSSING_OFFSET + 4 * i + 1]] << " ";
		stream << topvertices[vector[CROSSING_OFFSET + 4 * i + 2]] << " ";
		stream << bottomvertices[vector[CROSSING_OFFSET + 4 * i + 3]];
	}
}

bool vectors_equals(const CFINT* a, const CFINT* b)
{
	if (a == NULL)
		return b == NULL;
	if (b == NULL)
		return false;
		
	int len = getLength(a);

	for (int i = 0; i < len; i++)
		if (a[i] != b[i])
			return false;

	return true;
}

CFINT* copy_vector(const CFINT* vector)
{
	CFINT* new_vector = new CFINT[getLength(vector)];
	memcpy(new_vector, vector, sizeof(CFINT) * getLength(vector));
	SANITY_CHECK(new_vector);
	return new_vector;
}

void free_vector(CFINT* &vector)
{
	delete[] vector;
	vector = NULL;
}

std::size_t vector_hash_value(const CFINT* vector)
{
	if (vector == NULL) return 0;

	std::size_t seed = 0;
	int len = getLength(vector);

	for (int i = 0; i < len; i++)
	{
		// this statement is equivalent to boost::hash_combine(seed, vector[i]);
		seed ^= vector[i] + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}
	return seed;
}

void sanity_check_error(CFINT* vector, const std::string str, int line_no, const std::string fileName)
{
	std::cerr << "Sanity check '" << str << "' at " << fileName << ":" << line_no << " failed on: " << std::endl;
	print_vector(vector);
	exit(1);
}

void sanity_check(CFINT* vector, int line_no, const std::string fileName)
{
#ifdef DEBUG
	int N = getN(vector), K = getK(vector);
	int count[N+K];
	int crossingcount = getCrossingCount(vector);
	int len = getLength(vector);

	if (getNlabelled(vector) > N)
		sanity_check_error(vector, "Number of labelled top vertices should not exceed total number of top vertices", line_no, fileName);
	if (getKlabelled(vector) > K)
		sanity_check_error(vector, "Number of labelled bottom vertices should not exceed total number of bottom vertices", line_no, fileName);

	for (int i = 0; i < len; i++)
		if (vector[i] < 0)
			sanity_check_error(vector, "nonnegativity", line_no, fileName);  

	// check entries of crossings
	for (int i = 0; i < crossingcount; i++) { 
		CFINT* cr = &vector[CROSSING_OFFSET + 4 * i];
		if (cr[0] >= N)
			sanity_check_error(vector, "crossing top entry < N", line_no, fileName);  
		if (cr[1] >= K)
			sanity_check_error(vector, "crossing bottom entry < K", line_no, fileName);  
		if (cr[2] >= N)
			sanity_check_error(vector, "crossing top entry < N", line_no, fileName);  
		if (cr[3] >= K)
			sanity_check_error(vector, "crossing bottom entry < K", line_no, fileName);  
	}

	// check for duplicate crossings
	for (int i = 0; i < crossingcount; i++) { 
		for (int j = i+1; j < crossingcount; j++) { 
			CFINT* cr_i = &vector[CROSSING_OFFSET + 4 * i];
			CFINT* cr_j = &vector[CROSSING_OFFSET + 4 * j];
			if ((cr_i[0] == cr_j[0]) && (cr_i[1] == cr_j[1]) && (cr_i[2] == cr_j[2]) && (cr_i[3] == cr_j[3]))
				sanity_check_error(vector, "duplicate crossings", line_no, fileName); 
			if ((cr_i[0] == cr_j[2]) && (cr_i[1] == cr_j[3]) && (cr_i[2] == cr_j[0]) && (cr_i[3] == cr_j[1]))
				sanity_check_error(vector, "duplicate crossings", line_no, fileName); 
		}
	}
#endif // DEBUG
}

inline void sort_crossings(CFINT* vector)
{
	int crossingcount = getCrossingCount(vector);

	// first sort the endpoints of the edges
	for (int i = 0; i < crossingcount; i++)
		lex_sort(&vector[CROSSING_OFFSET + 4*i], 2, 2);

	lex_sort(&vector[CROSSING_OFFSET], crossingcount, 4);	
	SANITY_CHECK(vector);
}


void calc_canonical(CFINT* vector, bool permuteLabelledVertices)
{
	SANITY_CHECK(vector);
	int N = getN(vector), K = getK(vector), 
	    Nlabelled = getNlabelled(vector), 
	    Klabelled = getKlabelled(vector),
	    crossingcount = getCrossingCount(vector),
	    len = getLength(vector);

	CFINT permB[K], permA[N];

	CFINT new_vector[len], best_vector[len];

	int vectorBytes = sizeof(CFINT) * getLength(vector); // length of vector in bytes

	memcpy(best_vector, vector, vectorBytes);
	memcpy(new_vector,  vector, vectorBytes);

	for (CFINT n = 0; n < N; n++)
		permA[n] = n;

	int permutationCount = 0;
	do {
		do {
			for (CFINT k = 0; k < K; k++)
				permB[k] = k;
			do {
				do {
					// crossings start at position 5
					CFINT* new_ptr = &new_vector[CROSSING_OFFSET];   
					CFINT* ptr     = &vector[CROSSING_OFFSET];

					// Perform permutation. This code looks scarier than it is:
					// Each *(new_ptr++) refers to the current position in the
					// array, and moves the pointer by one position.
					for (int i = 0; i < crossingcount; i++)
					{
						*(new_ptr++) = permA[*(ptr++)];
						*(new_ptr++) = permB[*(ptr++)];
						*(new_ptr++) = permA[*(ptr++)];
						*(new_ptr++) = permB[*(ptr++)];
					}
	
					// sort the crossings
					sort_crossings(new_vector);

					// if the new crossings form a lexicographically smaller
					// vector than the previously best known, then 
					// save this vector as the new best

					permutationCount++;
					if (lex_compare(&new_vector[CROSSING_OFFSET], &best_vector[CROSSING_OFFSET], 4 * crossingcount) < 0)
						memcpy(best_vector, new_vector, vectorBytes);
				} while (advancePermutation(permB, Klabelled, K));
			} while (advancePermutation(permB, 0, permuteLabelledVertices ? Klabelled : 0));
		} while (advancePermutation(permA, Nlabelled, N));
	} while (advancePermutation(permA, 0, permuteLabelledVertices ? Nlabelled : 0));


	memcpy(vector, best_vector, vectorBytes);

	SANITY_CHECK(vector);
}



