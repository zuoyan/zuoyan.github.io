#include "array.h"

#include "double_array.h"

#include <stdio.h>

int test_failed = 0;

#define EXPECT(cond, format, ...)                                \
  if (!(cond)) {                                                 \
    fprintf(stderr,                                              \
            "%s %s:%d "                                          \
            "check '%s' failed:\n" format "\n",                  \
            __func__, __FILE__, __LINE__, #cond, ##__VA_ARGS__); \
    test_failed++;                                               \
  }

#define ASSERT(cond, format, ...)        \
  {                                      \
    int old_test_failed = test_failed;   \
    EXPECT(cond, format, ##__VA_ARGS__); \
    if (test_failed > old_test_failed) { \
      return;                            \
    }                                    \
  }

#define RUN_TEST(test)                         \
  fprintf(stderr, "==Testing %s ...\n", #test);   \
  test_failed = 0;                             \
  test();                                      \
  if (test_failed) {                           \
    fprintf(stderr, "==Failed %s\n\n", #test); \
  } else {                                     \
    fprintf(stderr, "==Passed %s\n\n", #test); \
  }

void TestArrayOfDouble() {
  Array a[1];
  TypeInfo double_info[1];
  TypeInfoInitPlain(double_info, sizeof(double));
  ArrayInit(a, 1, double_info);
  ASSERT(ArraySize(a) == 1, "array size(%zd) is not 1", ArraySize(a));
  ArrayResize(a, 10);
  ASSERT(ArraySize(a) == 10, "array size(%zd) is not 10", ArraySize(a));
  double sum_check = 0;
  for (size_t i = 0; i < 10; ++i) {
    double v = 1.0 / (i + 1);
    sum_check += v;
    double* p = ArrayGet(a, i);
    ASSERT(*p == 0, "init does not set zero, value %lf", *p);
    *(double*)ArrayGet(a, i) = v;
  }
  double s = 0;
  for (size_t i = 0; i < ArraySize(a); ++i) {
    double* p = ArrayGet(a, i);
    ASSERT(*p == 1.0 / (i + 1), "read got %lf does not match last set %lf", *p,
           1.0 / (i + 1));
    s += *(double*)ArrayGet(a, i);
  }
  ASSERT(s == sum_check, "sum(%lf) does not match(%lf)", s, sum_check);

  ArrayRelease(a);
}

void TestArrayOfArray() {
  Array a[1];
  TypeInfo double_info[1];
  TypeInfo array_info[1];
  TypeInfoInitPlain(double_info, sizeof(double));
  TypeInfoInitArray(array_info, double_info);

  size_t m = 40;
  ArrayInit(a, m - 1, array_info);
  ASSERT(ArraySize(a) == m - 1, "array size(%zd) is not %zd", ArraySize(a),
         m - 1);
  ArrayResize(a, m);
  ASSERT(ArraySize(a) == m, "array size(%zd) is not %zd", ArraySize(a), m);

  double sum_check = 0;
  for (size_t i = 0; i < m; ++i) {
    Array* row = (Array*)ArrayGet(a, i);
    ASSERT(ArraySize(row) == 0, "array size(%zd) is not 0", ArraySize(a));
    ArrayResize(row, i + 1);
    for (size_t j = 0; j <= i; ++j) {
      *(double*)ArrayGet(row, j) = 1.0 / (i + j + 1);
      sum_check += 1.0 / (i + j + 1);
    }
  }

  double s = 0;
  for (size_t i = 0; i < ArraySize(a); ++i) {
    Array* row = (Array*)ArrayGet(a, i);
    for (size_t j = 0; j < ArraySize(row); ++j) {
      s += *(double*)ArrayGet(row, j);
    }
  }
  ASSERT(s == sum_check, "sum(%lf) miss match %lf", s, sum_check);

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
  DoubleArrayRelease(d);
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

  size_t m = 40;
  ArrayInit(a, m - 1, double_array_info);
  ASSERT(ArraySize(a) == m - 1, "array size(%zd) is not %zd", ArraySize(a),
         m - 1);
  ArrayResize(a, m);
  ASSERT(ArraySize(a) == m, "array size(%zd) is not %zd", ArraySize(a), m);

  double sum_check = 0;
  for (size_t i = 0; i < m; ++i) {
    DoubleArray* row = (DoubleArray*)ArrayGet(a, i);
    ASSERT(DoubleArraySize(row) == 0, "array size(%zd) is not 0", ArraySize(a));
    DoubleArrayResize(row, i + 1);
    for (size_t j = 0; j <= i; ++j) {
      DoubleArraySet(row, j, 1.0 / (i + j + 1));
      sum_check += 1.0 / (i + j + 1);
    }
  }

  double s = 0;
  for (size_t i = 0; i < ArraySize(a); ++i) {
    DoubleArray* row = (DoubleArray*)ArrayGet(a, i);
    for (size_t j = 0; j < DoubleArraySize(row); ++j) {
      s += DoubleArrayGet(row, j);
    }
  }
  ASSERT(s == sum_check, "sum(%lf) miss match %lf", s, sum_check);

  ArrayRelease(a);
}

static int ExpectArrayDoubleEq(Array* a, const double* buf, size_t len) {
  int old_test_failed = test_failed;
  EXPECT(a->element_info_->size_ == sizeof(double),
         "The array has elment size %zd, not equal to double %zd",
         a->element_info_->size_, sizeof(double));
  EXPECT(ArraySize(a) == len, "array size %zd is not %zd", ArraySize(a), len);
  for (size_t i = 0, n = ArraySize(a); i < n && i < len; ++i) {
    EXPECT(*(double*)ArrayGet(a, i) == buf[i],
           "The value %lf at %zd is not %lf", *(double*)ArrayGet(a, i), i,
           buf[i]);
  }
  return old_test_failed == test_failed;
}

void TestArraySplice() {
  Array a[1], b[1];
  TypeInfo double_info[1];
  TypeInfoInitPlain(double_info, sizeof(double));
  ArrayInit(a, 0, double_info);
  ArrayInit(b, 0, double_info);
  ArraySplice(a, 0, 0, b, 0, 0);
  ASSERT(ArraySize(a) == 0, "a.size=%zd", ArraySize(a));
  ArrayInsertBuffer(a, 0, (double[]){1.1, 2.2, 3.3, 4.4}, 4);
  ArrayInsertBuffer(a, 4, (double[]){5.5, 6.6, 7.7}, 3);
  ASSERT(
      ExpectArrayDoubleEq(a, (double[]){1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7}, 7),
      "");
  ArraySplice(a, 0, 0, b, 0, 0);
  ASSERT(
      ExpectArrayDoubleEq(a, (double[]){1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7}, 7),
      "");
  ArraySplice(a, 1, 0, b, 0, 0);
  ASSERT(
      ExpectArrayDoubleEq(a, (double[]){1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7}, 7),
      "");
  ArraySplice(a, 1, 2, b, 0, 0);
  ASSERT(ExpectArrayDoubleEq(a, (double[]){1.1, 4.4, 5.5, 6.6, 7.7}, 5), "");

  ArrayInsertBuffer(b, 0, (double[]){1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7}, 7);

  ArraySplice(a, 1, 0, b, 1, 2);
  ASSERT(
      ExpectArrayDoubleEq(a, (double[]){1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7}, 7),
      "");

  ArraySplice(a, 1, 2, b, 4, 3);
  ASSERT(ExpectArrayDoubleEq(
             a, (double[]){1.1, 5.5, 6.6, 7.7, 4.4, 5.5, 6.6, 7.7}, 8),
         "");

  ArrayErase(a, 2, 6);
  ArrayAppend(a, (double[]){2.8});
  ASSERT(ExpectArrayDoubleEq(a, (double[]){1.1, 5.5, 2.8}, 3), "");

  ArrayPop(a);
  ASSERT(ExpectArrayDoubleEq(a, (double[]){1.1, 5.5}, 2), "");

  ArrayRelease(a);
  ArrayRelease(b);
}

int main(int argc, char* argv[]) {
  RUN_TEST(TestArrayOfDouble);

  RUN_TEST(TestArrayOfArray);

  RUN_TEST(TestArrayOfDoubleArray);

  RUN_TEST(TestArraySplice);

  return 0;
}
