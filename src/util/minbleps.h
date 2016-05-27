#pragma once

#include <cassert>
#include <cmath>
#include <complex>

#include "../config.h"
#include "../dssi/plugin.h"
#include "buf.h"

class MinBLEPs {

    int const _pcmRate;

    float const _naiveRate;

#ifdef DIZZYM_UNIT_TEST

public:

    Buffer<double> _BLI {"_BLI"};

    Buffer<std::complex<double>> _realCepstrum {"_realCepstrum"};

#endif

    sizex const _minBLEPCount;

    Buffer<float> _minBLEPs {"_minBLEPs"};

public:

    MinBLEPs(Config const&, int pcmRate);

    DSSI::cursor pcmXToNaiveX(DSSI::cursor pcmX) const {
        // First naiveX that doesn't affect pcmX prior to that given:
        return DSSI::cursor(floor(double(pcmX - 1) / _pcmRate * _naiveRate) + 1);
    }

    class Paster {

        View<float> const _minBLEPs;

        float const _naiveRate;

        sizex const _pcmRate, _minBLEPCount;

        sizex _pcmRelX;

#ifdef DIZZYM_UNIT_TEST
    public:
#endif

        sizex _minBLEPIndex;

    public:

        Paster(MinBLEPs const& minBLEPs)
                : _minBLEPs(minBLEPs._minBLEPs), //
                _naiveRate(minBLEPs._naiveRate), //
                _pcmRate(minBLEPs._pcmRate), //
                _minBLEPCount(minBLEPs._minBLEPCount), //
                _pcmRelX(), _minBLEPIndex() {
        }

        void pastePrepare(DSSI::cursor naiveX, DSSI::cursor pcmRef) {
            auto const pcmMark = double(naiveX) / _naiveRate * _pcmRate;
            // If pcmX is 1 too big due to rounding error, we simply skip _minBLEPs[0] which is close to zero:
            auto const pcmX = DSSI::cursor(ceil(pcmMark));
            assert(pcmRef <= pcmX);
            _pcmRelX = sizex(pcmX - pcmRef);
            auto const distance = double(pcmX) - pcmMark;
            _minBLEPIndex = sizex(round(distance * _minBLEPCount));
        }

        sizex minBLEPSize() const {
            return (_minBLEPs.limit() - _minBLEPIndex + _minBLEPCount - 1) / _minBLEPCount;
        }

        sizex pcmCountWithOverflow(DSSI::cursor naiveX, DSSI::cursor pcmRef) {
            pastePrepare(naiveX, pcmRef);
            return _pcmRelX + minBLEPSize();
        }

        void pasteMulti(View<float> derivative, DSSI::cursor naiveRef, DSSI::cursor pcmRef, View<float> pcmBuf) {
            auto const ampCount = derivative.limit();
            auto const ampPtr = derivative.begin();
            auto const lim = _minBLEPs.limit(), step = _minBLEPCount;
            auto const srcPtr = _minBLEPs.begin();
            for (sizex i = 0; i < ampCount; ++i) {
                auto const amp = ampPtr[i];
                if (amp) {
                    pastePrepare(naiveRef + i, pcmRef);
                    auto targetPtr = const_cast<float *>(pcmBuf.begin(_pcmRelX));
                    pcmBuf.begin(_pcmRelX + minBLEPSize()); // Bounds check.
                            // The target must be big enough for a minBLEP at maximum pcmX:
                    for (auto k = _minBLEPIndex; k < lim; k += step) {
                        *targetPtr++ += amp * srcPtr[k];
                    }
                    for (auto const end = pcmBuf.end(); targetPtr != end;) {
                        *targetPtr++ += amp;
                    }
                }
            }
        }

    };

};
