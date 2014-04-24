#ifndef __BRICKFLAG_H__
#define __BRICKFLAG_H__

#include <iostream>
#include "turan.h"

void calc_canonical(CFINT* vector, bool permuteLabelledVertices);
CFINT* copy_vector(const CFINT* vector);
void free_vector(CFINT* &vector);
bool vectors_equals(const CFINT* a, const CFINT* b);
std::size_t vector_hash_value(const CFINT* vector);

void print_vector(const CFINT* vector);
void print_vector(const CFINT* vector, std::ostream& stream);

/* Sanity checks */
#ifdef DEBUG
#define SANITY_CHECK(vector) { sanity_check(vector, __LINE__, __FILE__); }
#else
#define SANITY_CHECK(vector)
#endif

void sanity_check(CFINT* vector, int line_no, const std::string fileName);


#endif // __BRICKFLAG_H__
