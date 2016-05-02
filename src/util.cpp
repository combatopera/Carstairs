#include "util.h"

#include <ladspa.h>
#include <stdlib.h>
#include <cstring>

#ifdef DEBUG_dizzYM
#include <iostream>
#endif

template<typename T> View<T>::View() {
    _limit = 0;
    _data = (T *) malloc(0);
}

template<typename T> Buffer<T>::Buffer(const char *label) {
#ifdef DEBUG_dizzYM
    std::cerr << "[dizzYM] Creating Buffer: " << label << std::endl;
#endif
    _capacity = 0;
}

template<typename T> View<T>::View(const View<T>& master)
        : _limit(master._limit), _data(master._data) {
    // Nothing else.
}

template<typename T> Buffer<T>::~Buffer() {
    free(this->_data);
}

template<typename T> void Buffer<T>::setLimit(size_t limit) {
    if (limit > this->_capacity) {
        this->_data = (T *) realloc(this->_data, limit * sizeof(T));
        this->_capacity = limit;
    }
    this->_limit = limit;
}

template<typename T> void Buffer<T>::zero() {
    memset(this->_data, 0, this->_limit * sizeof(T)); // Not portable in float case.
}

template<typename T> void View<T>::copyTo(T *to) {
    memcpy(to, _data, _limit * sizeof(T));
}

UTIL_INSTANTIATE(int)
UTIL_INSTANTIATE(LADSPA_Data)
