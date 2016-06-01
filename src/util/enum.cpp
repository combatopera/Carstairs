#include "enum.h"

#include "../dssi/port.h"

template<typename T> Values<T>::Values(T *first, sizex ordCursor)
        : _first(first), length(ordCursor - _first->ordinal()) {
}

CARSTAIRS_ENUM_INSTANTIATE(PortInfo_t const)
