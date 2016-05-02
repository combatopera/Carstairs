#pragma once

#include <stddef.h>

#include "util.h"

template<typename T> class Values {

    T *_first;

public:

    size_t const _n;

    Values(T *first, index_t ordCursor);

    T *at(index_t i);

};

#define ENUM_INSTANTIATE(T) \
    template T *Values<T>::at(index_t); \
    template Values<T>::Values(T *, size_t);
