#pragma once

#include <stddef.h>

#include "util/util.h"

template<typename T> class View {

protected:

    size_t const INITIAL_CAPACITY = 0;

    View();

    size_t _limit;

    T *_data;

public:

    View(const View<T>& master);

    size_t limit() {
        return _limit;
    }

    void copyTo(T *to);

    T at(index_t i) {
        return _data[i];
    }

    void put(index_t i, T value) {
        _data[i] = value;
    }

    void fill(index_t i, index_t j, T value) {
        for (; i < j; ++i) {
            _data[i] = value;
        }
    }

    T *begin() {
        return _data;
    }

    T *end() {
        return _data + _limit;
    }

};

template<typename T> class Buffer: public View<T> {

    size_t _capacity;

    const char *_label;

public:

    Buffer(const char *label);

    ~Buffer();

    void setLimit(size_t limit);

    void zero();

};

#define BUF_INSTANTIATE(T) \
    template class View<T>; \
    template class Buffer<T>;
