#include "buf.h"

#include <fftw3.h>
#include <ladspa.h>
#include <stdlib.h>
#include <cmath>

#include "util.h"

template<typename T> View<T>::View(char const *label, sizex limit)
        : _limit(limit) {
    debug("Creating Buffer: %s", label);
    _data = (T *) malloc(limit * sizeof(T));
}

template<typename T> Buffer<T>::Buffer(char const *label, sizex limit)
        : View<T>(label, limit) {
    _capacity = limit;
    _label = label;
}

template<typename T> View<T>::View(View<T> const& master)
        : _limit(master._limit), _data(master._data) {
    // Nothing else.
}

template<typename T> Buffer<T>::~Buffer() {
    debug("Destroying Buffer: %s", _label);
    free(this->_data);
}

template<typename T> void Buffer<T>::setLimit(sizex limit) {
    if (limit > this->_capacity) {
        debug("Resizing %s to: %d", _label, limit);
        this->_data = (T *) realloc(this->_data, limit * sizeof(T));
        this->_capacity = limit;
    }
    this->_limit = limit;
}

template<> void View<double>::sinc() {
    for (auto i = _limit - 1; SIZEX_NEG != i; --i) {
        auto const x = M_PI * _data[i];
        _data[i] = x ? sin(x) / x : 1;
    }
}

template<> void View<double>::mul(sizex i, sizex j, double value) {
    for (; i < j; ++i) {
        _data[i] *= value;
    }
}

template<> void View<double>::mul(double const *values) {
    for (auto i = _limit - 1; SIZEX_NEG != i; --i) {
        _data[i] *= values[i];
    }
}

template<> void View<double>::blackman() {
    auto const N = _limit;
    assert(N & 1);
    auto const alpha = .16, a0 = (1 - alpha) / 2, a1 = .5, a2 = alpha / 2;
    auto const M = (N + 1) / 2; // Size of unique part.
    for (auto n = M - 1; SIZEX_NEG != n; --n) {
        _data[n] = a0 - a1 * cos(2 * M_PI * double(n) / double(N - 1)) + a2 * cos(4 * M_PI * double(n) / double(N - 1));
    }
    mirror(); // Avoid significant precision artifacts.
}

static double abs(std::complex<double> const& c) {
    return sqrt(c.real() * c.real() + c.imag() * c.imag());
}

template<> void View<double>::fillAbs(std::complex<double> const *values) {
    for (auto i = _limit - 1; SIZEX_NEG != i; --i) {
        _data[i] = abs(values[i]);
    }
}

template<> void View<double>::ln() {
    for (auto i = _limit - 1; SIZEX_NEG != i; --i) {
        _data[i] = log(_data[i]);
    }
}

template<> void View<double>::fillReal(std::complex<double> const *values) {
    for (auto i = _limit - 1; SIZEX_NEG != i; --i) {
        _data[i] = values[i].real();
    }
}

#define NUMERICS(T) template<> void View<T>::integrate() { \
    T sum = 0; \
    auto const n = _limit; \
    for (sizex i = 0; i < n; ++i) { \
        sum += _data[i]; \
        _data[i] = sum; \
    } \
} \
template<> void View<T>::differentiate(T context) { \
    for (auto i = _limit - 1; i > 0; --i) { \
        _data[i] -= _data[i - 1]; \
    } \
    _data[0] -= context; \
} \
template<> void View<T>::add(T value) { \
    for (auto i = _limit - 1; SIZEX_NEG != i; --i) { \
        _data[i] += value; \
    } \
} \
template<> void View<T>::mul(T value) { \
    for (auto i = _limit - 1; SIZEX_NEG != i; --i) { \
        _data[i] *= value; \
    } \
}

NUMERICS(float)
NUMERICS(double)
NUMERICS(int)
NUMERICS(std::complex<double>)

template<> void View<int>::range() {
    for (auto i = _limit - 1; SIZEX_NEG != i; --i) {
        _data[i] = int(i);
    }
}

template<> void View<float>::range() {
    for (auto i = _limit - 1; SIZEX_NEG != i; --i) {
        _data[i] = float(i);
    }
}

template<> void View<double>::range() {
    for (auto i = _limit - 1; SIZEX_NEG != i; --i) {
        _data[i] = double(i);
    }
}

template<> void View<std::complex<double>>::range() {
    for (auto i = _limit - 1; SIZEX_NEG != i; --i) {
        _data[i] = double(i);
    }
}

template<> void View<std::complex<double>>::mul(sizex i, sizex j, std::complex<double> value) {
    for (; i < j; ++i) {
        _data[i] *= value;
    }
}

template<> void View<std::complex<double>>::fillWidening(double const *values) {
    for (auto i = _limit - 1; SIZEX_NEG != i; --i) {
        _data[i] = values[i];
    }
}

template<> void View<float>::fillNarrowing(double const *values) {
    for (auto i = _limit - 1; SIZEX_NEG != i; --i) {
        _data[i] = float(values[i]);
    }
}

template<> void View<std::complex<double>>::fft() {
    auto plan = fftw_plan_dft_1d(int(_limit), reinterpret_cast<fftw_complex *>(_data), reinterpret_cast<fftw_complex *>(_data),
    FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(plan);
    fftw_destroy_plan(plan);
}

template<> void View<std::complex<double>>::ifft() {
    auto plan = fftw_plan_dft_1d(int(_limit), reinterpret_cast<fftw_complex *>(_data), reinterpret_cast<fftw_complex *>(_data),
    FFTW_BACKWARD, FFTW_ESTIMATE);
    fftw_execute(plan);
    fftw_destroy_plan(plan);
    mul(std::complex<double>(1 / double(_limit))); // How numpy does it.
}

template<> void View<std::complex<double>>::exp() {
    for (auto i = _limit - 1; SIZEX_NEG != i; --i) {
        _data[i] = std::exp(_data[i]);
    }
}

template<> void View<double>::rceps(Buffer<std::complex<double>>& fftAppliance, double addBeforeLog) {
    fftAppliance.setLimit(_limit);
    fftAppliance.fillWidening(begin());
    fftAppliance.fft();
    // Everything is real after we discard the phase info here:
    fillAbs(fftAppliance.begin());
    add(addBeforeLog); // Avoid taking log of zero. XXX: Why add not clamp?
    ln();
    fftAppliance.fillWidening(begin());
    fftAppliance.ifft();
    fillReal(fftAppliance.begin()); // MathWorks rceps does this too.
}

template<> void View<double>::minPhaseFromRceps(Buffer<std::complex<double>>& fftAppliance) {
    assert(!(_limit & 1));
    auto const midpoint = _limit / 2;
    // Leave first point, zero max phase part, double min phase part to compensate.
    // The midpoint is shared between parts so it doesn't change:
    mul(1, midpoint, 2);
    fill(midpoint + 1, _limit, double(0));
    fftAppliance.setLimit(_limit);
    fftAppliance.fillWidening(begin());
    fftAppliance.fft();
    fftAppliance.exp();
    fftAppliance.ifft();
    fillReal(fftAppliance.begin());
}

BUF_INSTANTIATE(int)
BUF_INSTANTIATE(LADSPA_Data)
BUF_INSTANTIATE(LADSPA_Data *)
BUF_INSTANTIATE(double)
BUF_INSTANTIATE(std::complex<double>)
