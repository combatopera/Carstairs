#include "buf.h"

#include <ladspa.h>
#include <stdlib.h>
#include <cstring>

#include "util/util.h"

template<typename T> View<T>::View() {
    _limit = 0;
    _data = (T *) malloc(0);
}

template<typename T> Buffer<T>::Buffer(const char *label) {
    debug("Creating Buffer: %s", label);
    _capacity = 0;
    _label = label;
}

template<typename T> View<T>::View(const View<T>& master)
        : _limit(master._limit), _data(master._data) {
    // Nothing else.
}

template<typename T> Buffer<T>::~Buffer() {
    debug("Destroying Buffer: %s", _label);
    free(this->_data);
}

template<typename T> void Buffer<T>::setLimit(size_t limit) {
    if (limit > this->_capacity) {
        debug("Resizing %s to: %d", _label, limit);
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

BUF_INSTANTIATE(int)
BUF_INSTANTIATE(LADSPA_Data)
BUF_INSTANTIATE(LADSPA_Data *)
