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
void* ArrayGet(Array* array, size_t i);

#endif  // FILE_77037415_78E5_414C_9AB9_A332A85C2B3B_H
