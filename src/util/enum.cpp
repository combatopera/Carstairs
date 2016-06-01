#include "enum.h"

#include "../dssi/port.h"

template<typename T> Values<T>::Values(T *first, sizex ordCursor)
        : _first(first), _n(ordCursor - _first->ordinal()) {
}

template<typename T> T *Values<T>::at(sizex i) const {
    return _first + i; // Assume the objects are right next to each other.
}

CARSTAIRS_ENUM_INSTANTIATE(PortInfo_t const)
