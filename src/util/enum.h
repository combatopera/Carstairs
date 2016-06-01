#pragma once

#include "buf.h"

template<typename T> class Values {

    T * const _first;

public:

    sizex const length;

    Values(T *first, sizex ordCursor);

    T *operator[](sizex i) const {
        return _first + i; // Assume the objects are right next to each other.
    }

};

#define CARSTAIRS_ENUM_INSTANTIATE(T) \
    template T *Values<T>::operator[](sizex) const; \
    template Values<T>::Values(T *, sizex);
