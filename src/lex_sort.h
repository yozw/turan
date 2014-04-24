#ifndef __LEX_SORT_H__
#define __LEX_SORT_H__

#include "turan.h"

// This function lexicographically compares two vectors a and b of 
// specified length. It returns:
//     -1 if a < b, 
//     +1 if a > b, 
//      0 if a = b.
int lex_compare(CFINT* a, CFINT* b, int length);

// This function lexicographically sorts an array in place. The input is 
// assumed to be contiguous and consisting of n vectors of length
// block_size. 
void lex_sort(CFINT* vector, int n, int block_size);


#endif // __LEX_SORT_H__
