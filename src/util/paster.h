#pragma once

#include <cassert>
#include <cmath>

#include "../dssi/plugin.h"
#include "buf.h"
#include "minbleps.h"

class Paster {

    double const _ratio;

    View<float> const _minBLEPs;

    sizex const _minBLEPCount;

    sizex _pcmRelX;

#ifdef DIZZYM_UNIT_TEST
public:
#endif

    sizex _minBLEPIndex;

    Paster(float naiveRate, int pcmRate, View<float> const minBLEPs, sizex minBLEPCount)
            : _ratio(double(pcmRate) / naiveRate), _minBLEPs(minBLEPs), _minBLEPCount(minBLEPCount), //
            _pcmRelX(), _minBLEPIndex() {
    }

public:

    Paster(MinBLEPs const& minBLEPs)
            : Paster(minBLEPs._naiveRate, minBLEPs._pcmRate, minBLEPs._minBLEPs, minBLEPs._minBLEPCount) {
    }

    void pastePrepare(DSSI::cursor naiveX, DSSI::cursor pcmRef) {
        auto const pcmMark = double(naiveX) * _ratio;
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
        auto const pcmEnd = pcmBuf.end();
        auto const pcmBegin = const_cast<float *>(pcmBuf.begin());
        for (sizex i = 0; i < ampCount; ++i) {
            auto const amp = ampPtr[i];
            if (amp) {
                pastePrepare(naiveRef + i, pcmRef);
                auto pcmPtr = pcmBegin + _pcmRelX;
                assert(pcmPtr + minBLEPSize() <= pcmEnd); // Bounds check.
                // The target must be big enough for a minBLEP at maximum pcmX:
                for (auto k = _minBLEPIndex; k < lim; k += step) {
                    *pcmPtr++ += amp * srcPtr[k];
                }
                while (pcmPtr != pcmEnd) {
                    *pcmPtr++ += amp;
                }
            }
        }
    }

};
