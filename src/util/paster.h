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

    inline void pastePrepare(DSSI::cursor naiveX, DSSI::cursor pcmRef, sizex& pcmRelX, sizex& minBLEPIndex) const {
        auto const pcmMark = double(naiveX) * _ratio;
        // If pcmX is 1 too big due to rounding error, we simply skip _minBLEPs[0] which is close to zero:
        auto const pcmX = DSSI::cursor(ceil(pcmMark));
        assert(pcmRef <= pcmX);
        pcmRelX = sizex(pcmX - pcmRef);
        auto const distance = double(pcmX) - pcmMark;
        minBLEPIndex = sizex(round(distance * _minBLEPCount));
    }

#ifdef DIZZYM_UNIT_TEST
public:
#endif

    Paster(float naiveRate, int pcmRate, View<float> const minBLEPs, sizex minBLEPCount)
            : _ratio(double(pcmRate) / naiveRate), _minBLEPs(minBLEPs), _minBLEPCount(minBLEPCount) {
    }

    inline sizex minBLEPSize(sizex minBLEPIndex) const {
        return (_minBLEPs.limit() - minBLEPIndex + _minBLEPCount - 1) / _minBLEPCount;
    }

public:

    Paster(MinBLEPs const& minBLEPs)
            : Paster(minBLEPs._naiveRate, minBLEPs._pcmRate, minBLEPs._minBLEPs, minBLEPs._minBLEPCount) {
    }

    sizex pcmCountWithOverflow(DSSI::cursor naiveX, DSSI::cursor pcmRef) const {
        sizex pcmRelX, minBLEPIndex;
        pastePrepare(naiveX, pcmRef, pcmRelX, minBLEPIndex);
        return pcmRelX + minBLEPSize(minBLEPIndex);
    }

    void pasteMulti(View<float> derivative, DSSI::cursor naiveRef, DSSI::cursor pcmRef, View<float> pcmBuf, sizex dcCursor, float dc) {
        pcmBuf.fill(dcCursor, pcmBuf.limit(), dc);
        auto const ampCount = derivative.limit();
        auto const ampPtr = derivative.begin();
        auto const lim = _minBLEPs.limit(), step = _minBLEPCount;
        auto const srcPtr = _minBLEPs.begin();
        auto const pcmEnd = pcmBuf.end();
        auto const pcmBegin = const_cast<float *>(pcmBuf.begin());
        for (sizex i = 0; i < ampCount; ++i) {
            auto const amp = ampPtr[i];
            if (amp) {
                sizex pcmRelX, minBLEPIndex;
                pastePrepare(naiveRef + i, pcmRef, pcmRelX, minBLEPIndex);
                auto pcmPtr = pcmBegin + pcmRelX;
                assert(pcmPtr + minBLEPSize(minBLEPIndex) <= pcmEnd); // Bounds check.
                // The target must be big enough for a minBLEP at maximum pcmX:
                for (auto k = minBLEPIndex; k < lim; k += step) {
                    *pcmPtr++ += amp * srcPtr[k];
                }
                while (pcmPtr != pcmEnd) {
                    *pcmPtr++ += amp;
                }
            }
        }
    }

};
