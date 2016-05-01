#pragma once

#include <stddef.h>

template<typename T> void zero(T *buf, size_t n);

template<typename T> void copy(T *from, T *to, size_t n);

#define INSTANTIATE(T) \
    template void zero<T>(T *, size_t n); \
    template void copy<T>(T *, T *, size_t n);
