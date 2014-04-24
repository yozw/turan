#include <iostream>
#include <string.h>

#include "lex_sort.h"

// Swap two vectors
inline void swap(CFINT* array1, CFINT* array2, int n) {
  assert(n >= 0);
  assert(array1 != NULL);
  assert(array2 != NULL);

  if (array1 == array2) return;

  int diff = array1 - array2;
  int vectorBytes = n * sizeof(CFINT);

  // if the arrays don't overlap, swapping can be done
  // with one temporary array
  if ((diff <= -n) || (diff >= n)) {
    CFINT temp[n];
    memcpy(temp,   array1, vectorBytes);
    memcpy(array1, array2, vectorBytes);
    memcpy(array2, temp,   vectorBytes);
  } else {
    // otherwise, we "need" two. (notice that this case
    // should not happen in the lexicographical sort
    // algorithm)
    CFINT temp1[n], temp2[n];
    memcpy(temp1, array1, vectorBytes);
    memcpy(temp2, array2, vectorBytes);

    memcpy(array1, temp2, vectorBytes);
    memcpy(array2, temp1, vectorBytes);
  }
}


// Compare two vectors lexicographically
int lex_compare(CFINT* a, CFINT* b, int length) {
  assert(length >= 0);
  assert(a != NULL);
  assert(b != NULL);

  for (int i = 0; i < length; i++) {
    if (*a < *b) return -1;
    if (*a > *b) return 1;
    a++;
    b++;
  }
  return 0;
}

// sorts an array lexicographically
void lex_sort(CFINT* vector, int n, int block_size) {
  assert(vector != NULL);
  assert(n >= 0);
  assert(block_size >= 0);

  if (n <= 1) return;

  if (n == 2) { // for n = 2, sorting is easy
    CFINT* block1 = vector;
    CFINT* block2 = vector + block_size;
    if (lex_compare(block1, block2, block_size) > 0)
      swap(block1, block2, block_size);
    return;
  }

  // choose pivot index for sort
  int pivotIndex = (int) n / 2;

  // move pivot element to end of list
  CFINT* pivot_block= &vector[block_size * (n-1)];
  swap(&vector[pivotIndex * block_size], pivot_block, block_size);

  // partition entries
  CFINT* block_i = vector;
  CFINT* store_block = vector;
  pivotIndex = 0;
  for (int i = 0; i < n - 1; i++) {
    if (lex_compare(block_i, pivot_block, block_size) < 0) {
      swap(block_i, store_block, block_size);
      store_block += block_size;
      pivotIndex++;
    }
    block_i += block_size;
  }

  swap(store_block, pivot_block, block_size);
  pivot_block = store_block;

  // recursively sort the two parts
  lex_sort(vector, pivotIndex, block_size);
  lex_sort(&pivot_block[block_size], n - pivotIndex - 1, block_size);


#ifdef DEBUG
  // check that the resulting array is properly sorted
  for (int i = 0; i < n - 1; i++) {
    if (lex_compare(&vector[block_size * i], &vector[block_size * (i+1)], block_size) > 0) {
      std::cerr << "Sorting algorithm failed!!" << std::endl;
      for (int j = 0; j < n; j++) {
        for (int k = 0; k < block_size; k++)
          std::cerr << vector[block_size * j + k] << " ";
        std::cerr << std::endl;
      }
    }
  }
#endif
}



