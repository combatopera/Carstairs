#include "enum.h"

#include "dssi/port.h"

template<typename T> Values<T>::Values(T *first, index_t ordCursor)
        : _first(first), _n(ordCursor - _first->_ordinal) {
}

template<typename T> T *Values<T>::at(index_t i) {
    return _first + i; // Assume the objects are right next to each other.
}

ENUM_INSTANTIATE(PortInfo_t)
