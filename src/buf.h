#pragma once

#include <stddef.h>

#include "util/util.h"

template<typename T> class View {

protected:

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

};

template<typename T> class Buffer: public View<T> {

    size_t _capacity;

    const char *_label;

public:

    Buffer(const char *label);

    ~Buffer();

    void setLimit(size_t limit);

    void zero();

    void put(index_t i, T value) {
        this->_data[i] = value;
    }

};

#define BUF_INSTANTIATE(T) \
    template class View<T>; \
    template class Buffer<T>;
