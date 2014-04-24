#ifndef __PERMUTATION_H__
#define __PERMUTATION_H__

#include <iostream>
#include <algorithm>
#include <assert.h>

template<class T> void quick_sort(T* elements, int n) {
  assert(elements != NULL);
  assert(n >= 0);

  if (n <= 1) return;

  if (n == 2) { // for n = 2, sorting is easy
    if (elements[0] > elements[1])
      std::swap<T>(elements[0], elements[1]);
    return;
  }

  // choose pivot index for sort
  int pivotIndex = (int) n / 2;

  // move pivot element to end of list
  T* pivot_element = &elements[n-1];
  std::swap<T>(elements[pivotIndex], *pivot_element);

  // partition entries
  T* element_i = elements;
  T* store_element = elements;
  pivotIndex = 0;
  for (int i = 0; i < n - 1; i++) {
    if (*element_i < *pivot_element) {
      std::swap<T>(*element_i, *store_element);
      store_element++;
      pivotIndex++;
    }
    element_i++;
  }

  std::swap<T>(*store_element, *pivot_element);
  pivot_element = store_element;

  // recursively sort the two parts
  quick_sort<T>(elements, pivotIndex);
  quick_sort<T>(pivot_element + 1, n - pivotIndex - 1);


#ifdef DEBUG
  // check that the resulting array is properly sorted
  for (int i = 0; i < n - 1; i++) {
    if (elements[i] > elements[i+1]) {
      std::cerr << "Sorting algorithm failed!!" << std::endl;
      for (int j = 0; j < n; j++)
        std::cerr << elements[j] << " ";
      std::cerr << std::endl;
    }
  }
#endif
}

template <class T> bool advancePermutation(T* a, int start, int end) {
  if (start >= end-1)
    return false;

  // Find the largest index k such that a[k] < a[k + 1]. If no such index exists,
  // the permutation is the last permutation.
  int k = end - 2;
  while (a[k] >= a[k+1]) {
    k--;
    if (k < start) return false;
  }

  // Find the largest index l such that a[k] < a[l]. Since k + 1 is such an index,
  // l is well defined and satisfies k < l.
  int l = end - 1;
  while (a[k] >= a[l])
    l--;

  // Swap a[k] with a[l].
  std::swap<T>(a[k], a[l]);

  // Reverse the sequence from a[k + 1] up to and including the final element a[n].
  for (int i = k+1; i <= (k+end)/2; i++)
    std::swap<T>(a[i], a[end+k-i]);

  return true;
}

template <class T> struct subsetBuffer {
  int subset_size, elements_size;
  T* elements;
  T* index;

  subsetBuffer(T* elements, const int elements_size, const int subset_size) {
    this->subset_size = subset_size;
    this->elements_size = elements_size;

    this->elements = new T[elements_size];
    for (int i = 0; i < elements_size; i++)
      this->elements[i] = elements[i];
    quick_sort<T> (this->elements, elements_size);


    this->index = NULL;
  }

  ~subsetBuffer() {
    delete[] this->elements;
    if (this->index != NULL)
      delete[] this->index;
  }
};


template <class T> bool __nextSubset__(T* subset, subsetBuffer<T>& buffer, bool ordered) {
  if (buffer.subset_size > buffer.elements_size)
    return false;

  if (buffer.index == NULL) {
    buffer.index = new T[buffer.subset_size];

    for (int i = 0; i < buffer.subset_size; i++)
      buffer.index[i] = i;
    for (int i = 0; i < buffer.subset_size; i++)
      subset[i] = buffer.elements[i];
    return true;
  }

  if (buffer.subset_size == 0)
    return false;

  if (ordered) {
    if (advancePermutation(subset, 0, buffer.subset_size))
      return true;
  }

  int increaseIndex = buffer.subset_size - 1;
  while (true) {
    buffer.index[increaseIndex]++;
    for (int i = increaseIndex + 1; i < buffer.subset_size; i++)
      buffer.index[i] = buffer.index[i - 1] + 1;

    if (buffer.index[buffer.subset_size - 1] < buffer.elements_size)
      break;

    if (increaseIndex == 0)
      return false;
    increaseIndex--;
  }

  if (ordered)
    increaseIndex = 0;

  for (int i = increaseIndex; i < buffer.subset_size; i++)
    subset[i] = buffer.elements[buffer.index[i]];

  return true;
}

template <class T> bool nextSubset(T* subset, subsetBuffer<T>& buffer) {
  return __nextSubset__(subset, buffer, false);
}

template <class T> bool nextUnorderedSubset(T* subset, subsetBuffer<T>& buffer) {
  return __nextSubset__(subset, buffer, false);
}

template <class T> bool nextOrderedSubset(T* subset, subsetBuffer<T>& buffer) {
  return __nextSubset__(subset, buffer, true);
}


inline int factorial(int n) {
  assert(n >= 0);

  int fact[] = {1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880, 3628800};
  if (n <= 10)
    return fact[n];
  else {
    int x = fact[10];
    for (int i = 11; i <= n; i++)
      x *= i;
    return x;
  }
}

inline int binomial(int n, int k) {
  assert(n >= 0);
  assert(k >= 0);

  if (k == 0) return 1;
  if (n == 0) return 0;

  return factorial(n) / factorial(k) / factorial(n-k);
}

#endif
