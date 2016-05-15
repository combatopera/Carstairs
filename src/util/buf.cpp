#include "buf.h"

#include <ladspa.h>
#include <stdlib.h>
#include <cmath>
#include <cstring>

#include "util.h"

template<typename T> View<T>::View() {
    _limit = INITIAL_CAPACITY;
    _data = (T *) malloc(INITIAL_CAPACITY * sizeof(T));
}

template<typename T> Buffer<T>::Buffer(const char *label) {
    debug("Creating Buffer: %s", label);
    _capacity = View<T>::INITIAL_CAPACITY;
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

template<> void View<double>::blackman() {
    size_t N = _limit;
    double alpha = .16, a0 = (1 - alpha) / 2, a1 = .5, a2 = alpha / 2;
    for (index_t n = 0; n < N; ++n) {
        _data[n] = a0 - a1 * cos(2 * M_PI * double(n) / double(N - 1)) + a2 * cos(4 * M_PI * double(n) / double(N - 1));
    }
}

BUF_INSTANTIATE(int)
BUF_INSTANTIATE(LADSPA_Data)
BUF_INSTANTIATE(LADSPA_Data *)