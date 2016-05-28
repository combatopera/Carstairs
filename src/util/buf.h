#pragma once

#include <cassert>
#include <complex>
#include <cstring>

typedef unsigned sizex;

sizex const SIZEX_NEG = sizex(0) - 1;

template<typename T> class Buffer;

template<typename T> class View {

protected:

    View(char const *, sizex limit);

    sizex _limit;

    T *_data;

public:

    View(View<T> const& master);

    sizex limit() const {
        return _limit;
    }

    void copyTo(T *to) const {
        memcpy(to, _data, _limit * sizeof(T));
    }

    T at(sizex i) const {
        return _data[i];
    }

    void put(sizex i, T value) {
        _data[i] = value;
    }

    void fill(sizex i, sizex j, T value) {
        for (; i < j; ++i) {
            _data[i] = value;
        }
    }

    void fill(sizex i, sizex j, T const *values) {
        memmove(_data + i, values, (j - i) * sizeof(T));
    }

    void fill(T value) {
        for (auto i = _limit - 1; SIZEX_NEG != i; --i) {
            _data[i] = value;
        }
    }

    void fill(T const *values) {
        memcpy(_data, values, _limit * sizeof(T));
    }

    void mirror() {
        assert(_limit & 1);
        auto const last = _limit - 1;
        for (auto i = last / 2 - 1; SIZEX_NEG != i; --i) {
            _data[last - i] = _data[i];
        }
    }

    void zero() {
        memset(_data, 0, _limit * sizeof(T)); // Not portable in float case.
    }

    void zero(sizex i, sizex j) {
        memset(_data + i, 0, (j - i) * sizeof(T)); // Not portable in float case.
    }

    T const *begin(sizex off = 0) const {
        assert(off <= _limit);
        return _data + off;
    }

    T const *end(sizex off = 0) const {
        assert(off <= _limit);
        return _data + _limit - off;
    }

    void differentiate(T context);

    void range();

    void sinc();

    void blackman();

    void mul(T value);

    void mul(sizex i, sizex j, T value);

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

    void rceps(Buffer<std::complex<double>>& fftAppliance, double addBeforeLog);

    void minPhaseFromRceps(Buffer<std::complex<double>>& fftAppliance);

};

template<typename T> class Buffer: public View<T> {

    sizex _capacity;

    char const *_label;

public:

    Buffer(char const *label, sizex limit = 0);

    ~Buffer();

    void setLimit(sizex limit);

    void pad(sizex left, sizex right, T value) {
        auto const mid = this->_limit;
        setLimit(left + mid + right);
        memmove(this->_data + left, this->_data, mid * sizeof(T));
        this->fill(0, left, value);
        this->fill(left + mid, this->_limit, value);
    }

    void snapshot(View<T> that) {
        setLimit(that.limit());
        this->fill(that.begin());
    }

};

#define BUF_INSTANTIATE(T) \
    template class View<T>; \
    template class Buffer<T>;
