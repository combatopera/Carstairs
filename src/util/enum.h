#pragma once

#include "buf.h"

template<typename T> class Values {

    T * const _first;

public:

    sizex const _n;

    Values(T *first, sizex ordCursor);

    T *at(sizex i) const;

};

#define CARSTAIRS_ENUM_INSTANTIATE(T) \
    template T *Values<T>::at(sizex) const; \
    template Values<T>::Values(T *, sizex);
