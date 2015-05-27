% Tutorial, Array
% Changsheng Jiang
% 2015-05-26

# Data structure, Array

In this tutorial, we define array interfaces and try to implement them in C.

# What is an Array

Array is a container holding multiple values of the same type, and can be read,
written with random offset.

Generally, array should support constant time complexity in random access(read
and write). An array that supports resize is a dynamic array, otherwise, it is a
static size array.

In C++, array is implemented as
[std::vector](http://en.cppreference.com/w/cpp/container/vector). In Python,
array is implemented as list. In C, you have to do it yourself.

Given any array type <code>Array</code>, it should support these methods:

* `void ArrayInit(Array* array, size_t size)`{.C}

    Initial the `array` conaining `size` elements.

    A impelementation might requires more information in the `ArrayInit`, such
    as the type information to manage the resource holded by elements.

* `void ArrayRelease(Array* array)`{.C}

    Release the `array`. After this, the `array` content is invalid. But a
    implementation might leave the `array` as initialized with zero.

* `size_t ArraySize(const Array* array)`{.C}

    Returns the number of elements stored in `array`.

* `void ArrayResize(const Array* array, size_t new_size)`{.C}

    Truncates or extends `array`. It is a fatal error if this
    failed. Implementation can change the interface to return boolean result.

* `void* ArrayGet(Array* array, size_t i)`{.C}

    Returns the adress of element at offset `i`.

    An offset should be less than the array size. It is a runtime error if an
    offset is larger than `ArraySize(array)`. Checking this or not, is a
    implementation behavior. It is usually good to add this check in debug mode
    for test, and disable the check in release mode for performance, see
    [assert](http://en.cppreference.com/w/cpp/error/assert) function.

A non general array type can have type information in these methods. For
example, an array of double can be defined by these accessing methods:

* `void DoubleArrayInit(DoubleArray* array, size_t size)`{.C}
* `void DoubleArrayRelease(DoubleArray* array)`{.C}
* `size_t DoubleArraySize(const DoubleArray* array)`{.C}
* `void DoubleArrayResize(DoubleArray* array, size_t new_size)`{.C}
* `double DoubleArrayGet(DoubleArray* array, size_t i)`{.C}
* `void DoubleArraySet(DoubleArray* array, size_t i, double v)`{.C}

Compare this list with the general array, we have simpler and type safe read and
write methods. Since it can only be used for double, the implementation can be
much simple, and has best performance. In C, a common approach to achieve type
safe and best runtime performance, without code duplication for every type, is
using preprocessor macro.

Based on these primitive methods, it is quite easy and straight to support more
methods, like append an element, insert an element, erase an element, append an
array, insert an array, erase an slice.

# Array in C

Let us start from the simpler version, a static plain type array.

## Single plain type Array

Following C++, We call a type is a plain type, if we do not need to construct
and destruct it, the data can be copied from one to another value by memory.

In C, we need to manage all memory manually. In the implementation of
`DoubleArray`, we need to call `DoubleArrayInit` to initialize an array, and
`DoubleArrayRelease` to release memory allocated intenrally by the array.

### File double_array.h
~~~ {.C .numberLines include=double_array.h}
~~~

## General type Array

To implement a general type Array, we need to abstract some common routines to
manage data of a dynamic type. We need pass these routines in the `ArrayInit`,
and in `ArrayRelease`, we also need to release resources allocated by array
elements. The initialization and release might also happen in array management,
like resize, append, erase.

We abstract a general type management routines as struct `TypeInfo`, and believe
this can be allocated statistically(or in stack), so a const pointer of TypeInfo
can be passed to `ArrayInit` without life time problem.

### File array.h

~~~ {.C .numberLines include=array.h}
~~~

### File array_test.c

~~~ {.C .numberLines include=array_test.c}
~~~

### File array.c

Before reading this file, you are encouraged to implement yourself version, to
make array_test pass, see [Problems](#problems) sections.

~~~ {.C .numberLines include=array.c}
array.c
~~~

# Compile and test

Assume we copy the relative code to file `double_array.h`, `array.h`, `array.c`
and `array_test.c` respectively, this code can be compiled and tested as:

~~~ {.bash}
# clang can be replacce by gcc, or cc
clang -std=c11 -Wall -O0 -g array_test.cc array.c -o array_test
./arary_test
# If valgrind is installed.
valgrind ./array_test
~~~

If you implement yourself version `array.c` in another file `array_ya.c`, then
it can be tested as:

~~~ {.bash}
clang -std=c11 -Wall -O0 -g array_test.cc array_ya.c -o array_ya_test
./arary_ya_test
# If valgrind is installed.
valgrind ./array_ya_test
~~~

# Problems

<span class="label label-default">Problem</span> This code is not tested, please
write some code to test this implementation.

<span class="label label-default">Problem</span> Implement `double
Sum(DoubleArray*array)`{.C}, returns the summation of all values in the array.

<span class="label label-default">Problem</span> Implement `size_t
ArgMax(DoubleArray*array)`{.C}, which returns the index of max value in the
array, and returns zero for empty array.

<span class="label label-default">Problem</span> Implement `void
DoubleArrayAppend(DoubleArray* array, double v)`{.C} based on the other access
methods in above, without touching `DoubleArray` fields.

<span class="label label-default">Problem</span> Since the `DoubleArray`'s
implementation is quite simple, we can copy code and rename the type for
`IntArray`, `Int64Array`. Try to define a macro `PLAIN_ARRAY(name, type)`, which
generate array code automatically. For example, `PLAIN_ARRAY(DoubleArray,
double)` can generate the code for `DoubleArray`, `PLAIN_ARRAY(IVec, int)`
generate code for int array, with all methods prefixed by IVec.

<span class="label label-default">Problem</span> Re-implement all `Array`{.C} methods.

<span class="label label-default">Problem</span> Implement a function to swap
two non interleaved ranges of array:

`void ArraySwapRanges(Array* array, size_t first_offset, size_t first_len,
size_t second_offset, size_t second_len)`{.C}

For example, for arary `[1, 2, 3, 4, 5, 6, 7, 8]`, calling
`ArraySwapRanges(array, 1, 2, 4, 3)`{.C} will modifiy the array into
`[1, 5, 6, 7, 4, 2, 3, 8]`.
