#include "util.h"

#include <ladspa.h>
#include <stddef.h>
#include <stdlib.h>
#include <cstdarg>
#include <cstdio>
#include <cstring>

#include "dssi/port.h"

template<typename T> Values<T>::Values(T *first, size_t n)
        : _first(first), _n(n) {
}

template<typename T> T *Values<T>::at(index_t i) {
    return _first + i; // Assume the objects are right next to each other.
}

void debug(const char *format, ...) {
#ifdef DEBUG_dizzYM
    fprintf(stderr, "[dizzYM] ");
    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);
    fprintf(stderr, "\n");
#endif
}

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

UTIL_INSTANTIATE(int)
UTIL_INSTANTIATE(LADSPA_Data)
UTIL_INSTANTIATE(LADSPA_Data *)
template PortInfo_t *Values<PortInfo_t>::at(index_t);
template Values<PortInfo_t>::Values(PortInfo_t *, size_t);
