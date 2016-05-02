#pragma once

#include <stddef.h>

typedef size_t index_t;

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

public:

    Buffer(const char *label);

    ~Buffer();

    void setLimit(size_t limit);

    void zero();

    void put(index_t i, T value) {
        this->_data[i] = value;
    }

};

#define UTIL_INSTANTIATE(T) \
    template class View<T>; \
    template class Buffer<T>;
