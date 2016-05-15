#pragma once

#include <stddef.h>
#include <cstring>

#include "util.h"

template<typename T> class View {

protected:

    View(const char *, size_t limit);

    size_t _limit;

    T *_data;

public:

    View(const View<T>& master);

    size_t limit() const {
        return _limit;
    }

    void copyTo(T *to);

    T at(index_t i) const {
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

    void range() {
        for (index_t i = 0, n = _limit; i < n; ++i) {
            _data[i] = (T) i;
        }
    }

    void sinc();

    void blackman();

    void mul(T value);

    void mul(T *values);

    void add(T value);

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

    Buffer(const char *label, size_t limit = 0);

    ~Buffer();

    void setLimit(size_t limit);

    void zeroPad(size_t left, size_t right) {
        size_t mid = this->_limit;
        setLimit(left + mid + right);
        memcpy(this->_data + left, this->_data, mid * sizeof(T));
        this->fill(0, left, 0);
        this->fill(left + mid, this->_limit, 0);
    }

    void zero();

};

#define BUF_INSTANTIATE(T) \
    template class View<T>; \
    template class Buffer<T>;
