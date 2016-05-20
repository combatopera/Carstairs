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

    void fill(T value) {
        for (index_t i = _limit - 1; SIZE_WRAP != i; --i) {
            _data[i] = value;
        }
    }

    void fill(T const *values) {
        for (index_t i = _limit - 1; SIZE_WRAP != i; --i) {
            _data[i] = values[i];
        }
    }

    void zero() {
        memset(_data, 0, _limit * sizeof(T)); // Not portable in float case.
    }

    T const *begin(int off = 0) const {
        if (off >= 0 && size_t(off) <= _limit) {
            return _data + off;
        }
        throw "out of bounds";
    }

    T const *end(int off = 0) const {
        if (off <= 0 && size_t(-off) <= _limit) {
            return _data + _limit + off;
        }
        throw "out of bounds";
    }

    void differentiate(T context);

    void range();

    void sinc();

    void blackman();

    void mul(T value);

    void mul(index_t i, index_t j, T value);

    void mul(T const *);

    void add(T value);

    void fillAbs(std::complex<double> const *);

    void fillReal(std::complex<double> const *);

    void ln();

    void integrate();

    void fillWidening(double const *);

    void fillNarrowing(double const *);

    void fft();

    void ifft();

    void exp();

};

template<typename T> class Buffer: public View<T> {

    size_t _capacity;

    char const *_label;

public:

    Buffer(char const *label, size_t limit = 0);

    ~Buffer();

    void setLimit(size_t limit);

    void pad(size_t left, size_t right, T value) {
        size_t mid = this->_limit;
        setLimit(left + mid + right);
        memcpy(this->_data + left, this->_data, mid * sizeof(T));
        this->fill(0, left, value);
        this->fill(left + mid, this->_limit, value);
    }

};

#define BUF_INSTANTIATE(T) \
    template class View<T>; \
    template class Buffer<T>;
