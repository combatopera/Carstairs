#pragma once

#include <stddef.h>
#include <cassert>
#include <cmath>
#include <complex>

#include "../config.h"
#include "buf.h"
#include "util.h"

class MinBLEPs {

    int const _pcmRate, _naiveRate;

#ifdef DIZZYM_UNIT_TEST

public:

    Buffer<double> _BLI {"_BLI"};

    Buffer<std::complex<double>> _realCepstrum {"_realCepstrum"};

#endif

    int const _minBLEPCount;

    Buffer<float> _minBLEPs {"_minBLEPs"};

    DSSI::cursor _pcmRelX;

    unsigned _minBLEPIndex;

public:

    MinBLEPs(Config const&);

    DSSI::cursor pcmXToNaiveX(DSSI::cursor pcmX) const {
        // First naiveX that doesn't affect pcmX prior to that given:
        return DSSI::cursor(floor(double(pcmX - 1) / _pcmRate * _naiveRate) + 1);
    }

    void pastePrepare(DSSI::cursor naiveX, DSSI::cursor pcmRef) {
        auto const pcmMark = double(naiveX) / _naiveRate * _pcmRate;
        // If pcmX is 1 too big due to rounding error, we simply skip _minBLEPs[0] which is close to zero:
        auto const pcmX = DSSI::cursor(ceil(pcmMark));
        assert(pcmRef <= pcmX);
        _pcmRelX = pcmX - pcmRef;
        auto const distance = double(pcmX) - pcmMark;
        _minBLEPIndex = unsigned(round(distance * _minBLEPCount));
    }

    size_t minBLEPSize() const {
        return (_minBLEPs.limit() - _minBLEPIndex + _minBLEPCount - 1) / _minBLEPCount;
    }

    size_t pcmCountWithOverflow() const {
        return _pcmRelX + minBLEPSize();
    }

    void pastePerform(float amp, View<float> pcmBuf) const {
        auto targetPtr = const_cast<float *>(pcmBuf.begin(_pcmRelX));
        pcmBuf.begin(pcmCountWithOverflow()); // Bounds check.
        // The target must be big enough for a minBLEP at maximum pcmX:
        for (index_t k = _minBLEPIndex, lim = _minBLEPs.limit(); k < lim; k += _minBLEPCount) {
            *targetPtr++ += amp * _minBLEPs.at(k);
        }
        for (auto const end = pcmBuf.end(); targetPtr != end;) {
            *targetPtr++ += amp;
        }
    }

};
