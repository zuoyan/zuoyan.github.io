#ifndef FILE_77037415_78E5_414C_9AB9_A332A85C2B3B_H
#define FILE_77037415_78E5_414C_9AB9_A332A85C2B3B_H

#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

typedef struct TypeInfo {
  // Common opaque data passed to all methods.
  void* data_;

  // Compile time size of this type.
  size_t size_;

  void (*init_)(void* data, void* p);

  void (*release_)(void* data, void* p);

  // Assume dest is already initialized. After this, resource managed by src is
  // moved to dest.
  void (*move_)(void* data, void* dest, void* src);

  // Assume dest is already initialized.
  void (*copy_)(void* data, void* dest, const void* src);
} TypeInfo;

// Initializes the TypeInfo for a plain type.
void TypeInfoInitPlain(TypeInfo* info, size_t size);

typedef struct Array {
  void* data_;
  size_t size_;
  size_t allocated_;
  const TypeInfo* element_info_;
} Array;

void TypeInfoInitArray(TypeInfo* info, const TypeInfo* element_info);

void ArrayInit(Array* array, size_t size, const TypeInfo* element_info);

void ArrayRelease(Array* array);

// Returns the size of the array, i.e. the number of elements in the array.
size_t ArraySize(const Array* array);

// Resize the array.
void ArrayResize(Array* array, size_t new_size);

// Returns the pointer of element at array with offset i.
void* ArrayGet(const Array* array, size_t i);

// Erase a sub range from array.
void ArrayErase(Array* array, size_t offset, size_t len);

// Insert a sub range with initialized values into array.
void ArrayInsertInitValues(Array* array, size_t offset, size_t len);

// Inserts a buffer into array. src should not be conatined by array.
void ArrayInsertBuffer(Array* array, size_t offset, const void* src,
                       size_t len);

// Inserts a buffer with values moved into array.
void ArrayMoveBuffer(Array* array, size_t offset, void* src, size_t len);

// Replace dest[dest_offset:dest_len] with src[0:src_len].
void ArraySpliceBuffer(Array* dest, size_t dest_offset, size_t dest_len,
                       const void* src, size_t src_len);

// Replace dest[dest_offset:dest_len] with src[0:src_len].
void ArraySpliceMoveBuffer(Array* dest, size_t dest_offset, size_t dest_len,
                           void* src, size_t src_len);

// Remove dest[dest_offset...], and copy src[src_offset...] into
// dest[dest_offset ...]
void ArraySplice(Array* dest, size_t dest_offset, size_t dest_len,
                 const Array* src, size_t src_offset, size_t src_len);

// Remove dest[dest_offset...], and move src[src_offset...] into
// dest[dest_offset ...]
void ArraySpliceMove(Array* dest, size_t dest_offset, size_t dest_len,
                     Array* src, size_t src_offset, size_t src_len);

// Remove dest[dest_offset...], and move src[src_offset...] into
// dest[dest_offset ...], and then erase src[src_offset...].
void ArraySpliceCut(Array* dest, size_t dest_offset, size_t dest_len,
                    Array* src, size_t src_offset, size_t src_len);

// Appends one value into the end of array.
void ArrayAppend(Array* array, void* value);

// Removes last value from array.
void ArrayPop(Array* array);

#endif  // FILE_77037415_78E5_414C_9AB9_A332A85C2B3B_H
