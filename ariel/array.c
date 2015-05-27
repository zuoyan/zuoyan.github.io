#include "array.h"

#include <stdint.h>

static void Plain_copy(void* data, void* dest, const void* src) {
  memcpy(dest, src, (intptr_t)data);
}

static void Plain_move(void* data, void* dest, void* src) {
  memmove(dest, src, (intptr_t)data);
}

static void Plain_init(void* data, void* p) { memset(p, 0, (intptr_t)data); }

static void Plain_release(void* data, void* p) { memset(p, 0, (intptr_t)data); }

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
static inline void* Offset(const void* data, size_t n) {
  return &((char*)data)[n];
}

static void Array_init(void* data, void* p) {
  ArrayInit(p, 0, (TypeInfo*)data);
}

static void Array_release(void* data, void* p) {
  assert(data == ((Array*)p)->element_info_);
  ArrayRelease(p);
}

static void Array_move(void* data, void* dest, void* src) {
  Array* d = (Array*)dest;
  Array* s = (Array*)src;
  assert(d != s);
  assert(data == ((Array*)d)->element_info_);
  assert(data == ((Array*)s)->element_info_);
  // Swap *d and *s.
  Array t = *d;
  *d = *s;
  *s = t;
}

static void Array_copy(void* data, void* dest, const void* src) {
  Array* d = (Array*)dest;
  Array* s = (Array*)src;
  assert(d != s);
  assert(data == ((Array*)d)->element_info_);
  assert(data == ((Array*)s)->element_info_);
  ArraySplice(d, 0, ArraySize(d), s, 0, ArraySize(s));
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

void* ArrayGet(const Array* array, size_t i) {
  assert(i < ArraySize(array));
  return Offset(array->data_, i * array->element_info_->size_);
}

void ArrayErase(Array* array, size_t offset, size_t len) {
  assert(offset + len <= ArraySize(array));
  if (!len) {
    return;
  }
  const TypeInfo* info = array->element_info_;
  size_t old_size = ArraySize(array);
  for (size_t i = offset + len; i < old_size; ++i) {
    info->move_(info->data_, Offset(array->data_, (i - len) * info->size_),
                Offset(array->data_, i * info->size_));
  }
  ArrayResize(array, old_size - len);
}

void ArrayInsertInitValues(Array* array, size_t offset, size_t len) {
  assert(offset <= ArraySize(array));
  if (!len) {
    return;
  }
  const TypeInfo* info = array->element_info_;
  size_t old_size = ArraySize(array);
  ArrayResize(array, old_size + len);
  for (size_t i = old_size; i-- > offset;) {
    info->move_(info->data_, Offset(array->data_, (i + len) * info->size_),
                Offset(array->data_, i * info->size_));
  }
}

void ArrayInsertBuffer(Array* array, size_t offset, const void* src,
                       size_t len) {
  assert(offset <= ArraySize(array));
  if (!len) {
    return;
  }
  const TypeInfo* info = array->element_info_;
  ArrayInsertInitValues(array, offset, len);
  for (size_t i = 0; i < len; ++i) {
    info->copy_(info->data_, Offset(array->data_, (i + offset) * info->size_),
                Offset(src, i * info->size_));
  }
}

void ArrayMoveBuffer(Array* array, size_t offset, void* src, size_t len) {
  assert(offset <= ArraySize(array));
  if (!len) {
    return;
  }
  const TypeInfo* info = array->element_info_;
  ArrayInsertInitValues(array, offset, len);
  for (size_t i = 0; i < len; ++i) {
    info->move_(info->data_, Offset(array->data_, (i + offset) * info->size_),
                Offset(src, i * info->size_));
  }
}

void ArraySpliceBuffer(Array* dest, size_t dest_offset, size_t dest_len,
                       const void* src, size_t src_len) {
  assert(dest_offset + dest_len <= ArraySize(dest));
  const TypeInfo* info = dest->element_info_;
  size_t old_size = ArraySize(dest);
  size_t common_len = dest_len < src_len ? dest_len : src_len;
  for (size_t i = 0; i < common_len; ++i) {
    info->copy_(info->data_,
                Offset(dest->data_, (i + dest_offset) * info->size_),
                Offset(src, i * info->size_));
  }
  if (src_len < dest_len) {
    ArrayErase(dest, dest_offset + src_len, dest_len - src_len);
  } else if (src_len > dest_len) {
    ArrayInsertBuffer(dest, dest_offset + dest_len,
                      Offset(src, dest_len * info->size_), src_len - dest_len);
  }
  assert(ArraySize(dest) == old_size + src_len - dest_len);
}

void ArraySpliceMoveBuffer(Array* dest, size_t dest_offset, size_t dest_len,
                           void* src, size_t src_len) {
  assert(dest_offset + dest_len <= ArraySize(dest));
  const TypeInfo* info = dest->element_info_;
  size_t old_size = ArraySize(dest);
  size_t common_len = dest_len < src_len ? dest_len : src_len;
  for (size_t i = 0; i < common_len; ++i) {
    info->move_(info->data_,
                Offset(dest->data_, (i + dest_offset) * info->size_),
                Offset(src, i * info->size_));
  }
  if (src_len < dest_len) {
    ArrayErase(dest, dest_offset + src_len, dest_len - src_len);
  } else if (src_len > dest_len) {
    ArrayMoveBuffer(dest, dest_offset + dest_len,
                    Offset(src, dest_len * info->size_), src_len - dest_len);
  }
  assert(ArraySize(dest) == old_size + src_len - dest_len);
}

void ArraySplice(Array* dest, size_t dest_offset, size_t dest_len,
                 const Array* src, size_t src_offset, size_t src_len) {
  assert(dest_offset + dest_len <= ArraySize(dest));
  assert(dest->element_info_ == src->element_info_);
  assert(src_offset + src_len <= ArraySize(src));
  assert(dest->element_info_ == src->element_info_);
  ArraySpliceBuffer(dest, dest_offset, dest_len,
                    src_len ? ArrayGet(src, src_offset) : NULL, src_len);
}

void ArraySpliceMove(Array* dest, size_t dest_offset, size_t dest_len,
                     Array* src, size_t src_offset, size_t src_len) {
  assert(dest_offset + dest_len <= ArraySize(dest));
  assert(dest->element_info_ == src->element_info_);
  assert(src_offset + src_len <= ArraySize(src));
  assert(dest->element_info_ == src->element_info_);
  ArraySpliceMoveBuffer(dest, dest_offset, dest_len,
                        src_len ? ArrayGet(src, src_offset) : NULL, src_len);
}

void ArraySpliceCut(Array* dest, size_t dest_offset, size_t dest_len,
                    Array* src, size_t src_offset, size_t src_len) {
  ArraySpliceMove(dest, dest_offset, dest_len, src, src_offset, src_len);
  ArrayErase(src, src_offset, src_len);
}

void ArrayAppend(Array* a, void* value) {
  ArrayInsertBuffer(a, ArraySize(a), value, 1);
}

void ArrayPop(Array* array) {
  assert(ArraySize(array) > 0);
  ArrayResize(array, ArraySize(array) - 1);
}
