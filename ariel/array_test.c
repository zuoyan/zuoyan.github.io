#include "array.h"

#include "double_array.h"

#include <stdio.h>

void TestArrayOfDouble() {
  Array a[1];
  TypeInfo double_info[1];
  TypeInfoInitPlain(double_info, sizeof(double));
  ArrayInit(a, 1, double_info);
  assert(ArraySize(a) == 1);
  ArrayResize(a, 10);
  assert(ArraySize(a) == 10);
  for (size_t i = 0; i < 10; ++i) {
    double v = 1.0 / (i + 1);
    *(double*)ArrayGet(a, i) = v;
  }
  double s = 0;
  for (size_t i = 0; i < ArraySize(a); ++i) {
    s += *(double*)ArrayGet(a, i);
  }

  printf("In func %s(line %d), sum=%lf\n", __func__, __LINE__, s);

  ArrayRelease(a);
}

void TestArrayOfArray() {
  Array a[1];
  TypeInfo double_info[1];
  TypeInfo array_info[1];
  TypeInfoInitPlain(double_info, sizeof(double));
  TypeInfoInitArray(array_info, double_info);

  size_t m = 40, n = 3;
  ArrayInit(a, m - 1, array_info);

  ArrayResize(a, m);

  for (size_t i = 0; i < m; ++i) {
    Array* row = (Array*)ArrayGet(a, i);
    ArrayResize(row, n);
    for (size_t j = 0; j < n; ++j) {
      *(double*)ArrayGet(row, j) = 1.0 / (i + j + 1);
    }
  }

  double s = 0;
  for (size_t i = 0; i < ArraySize(a); ++i) {
    Array* row = (Array*)ArrayGet(a, i);
    for (size_t j = 0; j < ArraySize(row); ++j) {
      s += *(double*)ArrayGet(row, j);
    }
  }

  printf("In func %s(line %d), sum=%lf\n", __func__, __LINE__, s);

  ArrayRelease(a);
}

void DoubleArray_init(void* data, void* p) {
  DoubleArrayInit((DoubleArray*)p, 0);
}

void DoubleArray_release(void* data, void* p) {
  DoubleArrayRelease((DoubleArray*)p);
}

void DoubleArray_move(void* data, void* dest, void* src) {
  DoubleArray* d = (DoubleArray*)dest;
  DoubleArray* s = (DoubleArray*)src;
  memmove(d, s, sizeof(DoubleArray));
  DoubleArrayInit(s, 0);
}

void DoubleArray_copy(void* data, void* dest, const void* src) {
  DoubleArray* d = (DoubleArray*)dest;
  DoubleArray* s = (DoubleArray*)src;
  DoubleArrayRelease(d);
  DoubleArrayInit(d, DoubleArraySize(s));
  for (size_t i = 0,n = DoubleArraySize(d); i < n; ++i) {
    DoubleArraySet(d, i, DoubleArrayGet(s, i));
  }
}

void TypeInfoInitDoubleArray(TypeInfo* info) {
  TypeInfoInitPlain(info, sizeof(DoubleArray));
  info->init_ = DoubleArray_init;
  info->release_ = DoubleArray_release;
  info->move_ = DoubleArray_move;
  info->copy_ = DoubleArray_copy;
}

void TestArrayOfDoubleArray() {
  Array a[1];
  TypeInfo double_array_info[1];
  TypeInfoInitDoubleArray(double_array_info);

  size_t m = 40, n = 3;
  ArrayInit(a, m - 1, double_array_info);

  ArrayResize(a, m);

  for (size_t i = 0; i < m; ++i) {
    DoubleArray* row = (DoubleArray*)ArrayGet(a, i);
    DoubleArrayResize(row, n);
    for (size_t j = 0; j < n; ++j) {
      DoubleArraySet(row, j, 1.0 / (i + j + 1));
    }
  }

  double s = 0;
  for (size_t i = 0; i < ArraySize(a); ++i) {
    DoubleArray* row = (DoubleArray*)ArrayGet(a, i);
    for (size_t j = 0; j < DoubleArraySize(row); ++j) {
      s += DoubleArrayGet(row, j);
    }
  }

  printf("In func %s(line %d), sum=%lf\n", __func__, __LINE__, s);

  ArrayRelease(a);
}

int main(int argc, char* argv[]) {
  TestArrayOfDouble();

  TestArrayOfArray();

  TestArrayOfDoubleArray();

  return 0;
}
