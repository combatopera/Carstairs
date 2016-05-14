#pragma once

#include <stddef.h>

#include "util.h"

template<typename T> class Values {

    T * const _first;

public:

    size_t const _n;

    Values(T *first, index_t ordCursor);

    T *at(index_t i) const;

};

#define ENUM_INSTANTIATE(T) \
    template T *Values<T>::at(index_t) const; \
    template Values<T>::Values(T *, size_t);
