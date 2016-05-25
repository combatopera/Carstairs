#pragma once

#include <cassert>
#include <cmath>
#include <complex>

#include "../config.h"
#include "../dssi/plugin.h"
#include "buf.h"

class MinBLEPs {

    int const _pcmRate, _naiveRate;

#ifdef DIZZYM_UNIT_TEST

public:

    Buffer<double> _BLI {"_BLI"};

    Buffer<std::complex<double>> _realCepstrum {"_realCepstrum"};

#endif

    sizex const _minBLEPCount;

    Buffer<float> _minBLEPs {"_minBLEPs"};

public:

    MinBLEPs(Config const&);

    DSSI::cursor pcmXToNaiveX(DSSI::cursor pcmX) const {
        // First naiveX that doesn't affect pcmX prior to that given:
        return DSSI::cursor(floor(double(pcmX - 1) / _pcmRate * _naiveRate) + 1);
    }

    class Paster {

        MinBLEPs const& _minBLEPs;

        sizex _pcmRelX, _minBLEPIndex;

    public:

        Paster(MinBLEPs const& minBLEPs)
                : _minBLEPs(minBLEPs), _pcmRelX(), _minBLEPIndex() {
        }

        void pastePrepare(DSSI::cursor naiveX, DSSI::cursor pcmRef) {
            auto const pcmMark = double(naiveX) / _minBLEPs._naiveRate * _minBLEPs._pcmRate;
            // If pcmX is 1 too big due to rounding error, we simply skip _minBLEPs[0] which is close to zero:
            auto const pcmX = DSSI::cursor(ceil(pcmMark));
            assert(pcmRef <= pcmX);
            _pcmRelX = sizex(pcmX - pcmRef);
            auto const distance = double(pcmX) - pcmMark;
            _minBLEPIndex = sizex(round(distance * _minBLEPs._minBLEPCount));
        }

        sizex minBLEPSize() const {
            return (_minBLEPs._minBLEPs.limit() - _minBLEPIndex + _minBLEPs._minBLEPCount - 1) / _minBLEPs._minBLEPCount;
        }

        sizex pcmCountWithOverflow() const {
            return _pcmRelX + minBLEPSize();
        }

        void pastePerform(float amp, View<float> pcmBuf) const {
            auto targetPtr = const_cast<float *>(pcmBuf.begin(_pcmRelX));
            pcmBuf.begin(pcmCountWithOverflow()); // Bounds check.
            // The target must be big enough for a minBLEP at maximum pcmX:
            auto const lim = _minBLEPs._minBLEPs.limit();
            for (auto k = _minBLEPIndex; k < lim; k += _minBLEPs._minBLEPCount) {
                *targetPtr++ += amp * _minBLEPs._minBLEPs.at(k);
            }
            for (auto const end = pcmBuf.end(); targetPtr != end;) {
                *targetPtr++ += amp;
            }
        }

    };

};
