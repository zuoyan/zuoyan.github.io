#include "array.h"

#include <stdint.h>

static void Plain_copy(void* data, void* dest, const void* src) {
  memcpy(dest, src, (intptr_t)data);
}

static void Plain_move(void* data, void* dest, void* src) {
  memmove(dest, src, (intptr_t)data);
}

static void Plain_init(void* data, void* p) {}

static void Plain_release(void* data, void* p) {}

void TypeInfoInitPlain(TypeInfo* info, size_t size) {
  memset(info, 0, sizeof(TypeInfo));
  info->data_ = (void*)(intptr_t)size;
  info->size_ = size;
  info->init_ = Plain_init;
  info->release_ = Plain_release;
  info->copy_ = Plain_copy;
  info->move_ = Plain_move;
}

// A help routine.
static inline void* Offset(void* data, size_t n) { return &((char*)data)[n]; }

static void Array_init(void* data, void* p) {
  ArrayInit(p, 0, (TypeInfo*)data);
}

static void Array_release(void* data, void* p) { ArrayRelease(p); }

static void Array_move(void* data, void* dest, void* src) {
  Array* d = (Array*)dest;
  Array* s = (Array*)src;
  assert(d != s);
  ArrayRelease(d);
  memcpy(dest, src, sizeof(Array));
  memset(src, 0, sizeof(Array));
}

static void Array_copy(void* data, void* dest, const void* src) {
  Array* d = (Array*)dest;
  Array* s = (Array*)src;
  assert(d != s);
  ArrayRelease(d);
  ArrayInit(d, ArraySize(s), s->element_info_);
  const TypeInfo* info = s->element_info_;
  for (size_t i = 0, n = ArraySize(d); i < n; ++i) {
    info->copy_(info->data_, Offset(d->data_, i * info->size_),
                Offset(s->data_, i * info->size_));
  }
}

void TypeInfoInitArray(TypeInfo* info, const TypeInfo* element_info) {
  TypeInfoInitPlain(info, sizeof(Array));
  info->data_ = (void*)element_info;
  info->init_ = Array_init;
  info->release_ = Array_release;
  info->copy_ = Array_copy;
  info->move_ = Array_move;
}

void ArrayInit(Array* array, size_t size, const TypeInfo* element_info) {
  memset(array, 0, sizeof(Array));
  array->element_info_ = element_info;
  if (size > 0) {
    ArrayResize(array, size);
  }
}

void ArrayRelease(Array* array) {
  // Resize to 0 to release elements.
  ArrayResize(array, 0);
  free(array->data_);
  // Leave an empty valid array.
  ArrayInit(array, 0, array->element_info_);
}

size_t ArraySize(const Array* array) { return array->size_; }

void ArrayResize(Array* array, size_t new_size) {
  const TypeInfo* info = array->element_info_;
  if (new_size <= array->allocated_) {
    size_t old_size = array->size_;
    for (size_t i = new_size; i < old_size; ++i) {
      info->release_(info->data_, Offset(array->data_, i * info->size_));
    }
    for (size_t i = old_size; i < new_size; ++i) {
      info->init_(info->data_, Offset(array->data_, i * info->size_));
    }
    array->size_ = new_size;
    return;
  }
  size_t allocation = new_size;
  if (allocation < array->size_ * 2) {
    allocation = array->size_ * 2;
  }
  void* p = malloc(allocation * info->size_);
  assert(p);
  for (size_t i = 0; i < new_size; ++i) {
    info->init_(info->data_, Offset(p, i * info->size_));
  }
  size_t common_size = new_size < array->size_ ? new_size : array->size_;
  for (size_t i = 0; i < common_size; ++i) {
    info->move_(info->data_, Offset(p, i * info->size_),
                Offset(array->data_, i * info->size_));
  }
  for (size_t i = 0; i < array->size_; ++i) {
    info->release_(info->data_, Offset(array->data_, i * info->size_));
  }
  free(array->data_);
  array->data_ = p;
  array->size_ = new_size;
  array->allocated_ = allocation;
}

void* ArrayGet(Array* array, size_t i) {
  assert(i < ArraySize(array));
  return Offset(array->data_, i * array->element_info_->size_);
}
