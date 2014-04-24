#include <iostream>
#include <string.h>

using namespace std;

typedef unsigned short CFINT;

CFINT array[] = {  0, 1, 1, 2, 0, 2, 2, 1, 1, 1, 2, 0, 0, 1, 2, 0, 0, 2, 2, 0, 0, 2, 1, 0, 0, 1, 1, 0 };

inline void swap(CFINT* array1, CFINT* array2, int n) {
  if (array1 == array2) return;
  CFINT temp1[n];
  CFINT temp2[n];
  memcpy(temp1, array1, n * sizeof(CFINT));
  memcpy(temp2, array2, n * sizeof(CFINT));

  memcpy(array1, temp2, n * sizeof(CFINT));
  memcpy(array2, temp1, n * sizeof(CFINT));
}

void print_array(CFINT* array, int n) {
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < 4; j++)
      cout << array[4 * i + j] << " ";
    cout << endl;
  }
  cout << endl;
}

int lex_compare(CFINT* a, CFINT* b, int length) {
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
  if (n <= 1) return;

  if (n == 2) {
    CFINT* block1 = vector;
    CFINT* block2 = vector + block_size;
    if (lex_compare(block1, block2, block_size) > 0)
      swap(block1, block2, block_size);
    return;
  }

  // choose pivot index for sort
  int pivotIndex = (int) n / 2;

  cout << "lex_sort(" << n << ")" <<endl;

  // move pivot element to end of list
  CFINT* pivot_block= &vector[block_size * (n-1)];
  swap(&vector[pivotIndex * block_size], pivot_block, block_size);

  cout << "after moving pivot element to end:" << endl;
  print_array(vector, n);

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

  cout << "after partitioning, pivotIndex = " << pivotIndex << endl;
  print_array(vector, n);

  // recursively sort the two parts
  lex_sort(vector, pivotIndex, block_size);
  lex_sort(&pivot_block[block_size], n - pivotIndex - 1, block_size);
}



int main() {
  print_array(array, 7);
  lex_sort(array, 7, 4);
  print_array(array, 7);
}
