#pragma once

#include <stddef.h>
#include <complex>
#include <cstring>

#include "util.h"

template<typename T> class View {

protected:

    View(char const *, size_t limit);

    size_t _limit;

    T *_data;

public:

    View(View<T> const& master);

    size_t limit() const {
        return _limit;
    }

    void copyTo(T *to) const {
        memcpy(to, _data, _limit * sizeof(T));
    }

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

    void fill(double const *values);

    void fill(T value) {
        for (index_t i = _limit - 1; SIZE_WRAP != i; --i) {
            _data[i] = value;
        }
    }

    void range() {
        for (index_t i = _limit - 1; SIZE_WRAP != i; --i) {
            _data[i] = (T) i;
        }
    }

    void sinc();

    void blackman();

    void mul(T value);

    void mul(index_t i, index_t j, T value);

    void mul(T const *values);

    void add(T value);

    void fillAbs(std::complex<double> const *);

    void fft();

    void exp();

    void fillReal(std::complex<double> const *);

    void ln();

    void ifft();

    void integrate();

    void zero() {
        memset(_data, 0, _limit * sizeof(T)); // Not portable in float case.
    }

    T const *begin() const {
        return _data;
    }

    T const *end() const {
        return _data + _limit;
    }

};

template<typename T> class Buffer: public View<T> {

    size_t _capacity;

    char const *_label;

public:

    Buffer(char const *label, size_t limit = 0);

    ~Buffer();

    void setLimit(size_t limit);

    void zeroPad(size_t left, size_t right) {
        size_t mid = this->_limit;
        setLimit(left + mid + right);
        memcpy(this->_data + left, this->_data, mid * sizeof(T));
        this->fill(0, left, 0);
        this->fill(left + mid, this->_limit, 0);
    }

};

#define BUF_INSTANTIATE(T) \
    template class View<T>; \
    template class Buffer<T>;
