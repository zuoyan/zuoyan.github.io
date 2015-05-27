#ifndef FILE_236B6066_3B25_4C9D_A3B2_D2AA3F7827CA_H
#define FILE_236B6066_3B25_4C9D_A3B2_D2AA3F7827CA_H

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

typedef struct DoubleArray {
  double* data_;
  size_t size_;
  size_t allocated_;
} DoubleArray;

static inline void DoubleArrayResize(DoubleArray* array, size_t new_size);

static inline void DoubleArrayInit(DoubleArray* array, size_t size) {
  memset(array, 0, sizeof(*array));
  if (size > 0) {
    DoubleArrayResize(array, size);
  }
}

static inline void DoubleArrayRelease(DoubleArray* array) {
  free(array->data_);
  memset(array, 0, sizeof(*array));
}

static inline size_t DoubleArraySize(const DoubleArray* array) {
  return array->size_;
}

static inline void DoubleArrayResize(DoubleArray* array, size_t new_size) {
  if (new_size <= array->allocated_) {
    array->size_ = new_size;
    return;
  }
  size_t allocation = new_size;
  if (allocation < array->size_ * 2) {
    allocation = array->size_ * 2;
  }
  void* p = realloc(array->data_, allocation * sizeof(double));
  assert(p);
  array->data_ = (double*)p;
  array->size_ = new_size;
  array->allocated_ = allocation;
}

static inline double DoubleArrayGet(DoubleArray* array, size_t i) {
  assert(i < DoubleArraySize(array));
  return array->data_[i];
}

static inline void DoubleArraySet(DoubleArray* array, size_t i, double v) {
  assert(i < DoubleArraySize(array));
  array->data_[i] = v;
}

#endif  // FILE_236B6066_3B25_4C9D_A3B2_D2AA3F7827CA_H
