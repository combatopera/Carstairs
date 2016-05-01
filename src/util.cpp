#include "util.h"

#include <ladspa.h>
#include <stdlib.h>
#include <cstring>

template<typename T> Buf<T>::Buf()
        : _master(true) {
    _data = (T *) malloc(_limit = _capacity = 0);
}

template<typename T> Buf<T>::Buf(const Buf<T>& master)
        : _master(false), _limit(master._limit), _capacity(master._capacity), _data(master._data) {
    // Nothing else.
}

template<typename T> Buf<T>::~Buf() {
    if (_master) {
        free(_data);
    }
}

template<typename T> void MasterBuf<T>::setLimit(size_t limit) {
    if (limit > this->_capacity) {
        this->_data = (T *) realloc(this->_data, limit * sizeof(T));
        this->_capacity = limit;
    }
    this->_limit = limit;
}

template<typename T> size_t Buf<T>::limit() {
    return _limit;
}

template<typename T> void MasterBuf<T>::zero() {
    memset(this->_data, 0, this->_limit * sizeof(T)); // Not portable in float case.
}

template<typename T> void Buf<T>::copy(T *to) {
    memcpy(to, _data, _limit * sizeof(T));
}

UTIL_INSTANTIATE(int)
UTIL_INSTANTIATE(LADSPA_Data)
