#pragma once

#include <cassert>
#include <cmath>

#include "../dssi/plugin.h"
#include "buf.h"
#include "minbleps.h"

#ifdef CARSTAIRS_TEST
double getPcmMark(Paster const& paster, DSSI::cursor naiveCursor);
#endif

class Paster {

    double const _ratio;

    View<float> const _minBLEPs;

    sizex const _minBLEPCount;

    inline void pastePrepare(DSSI::cursor naiveCursor, DSSI::cursor pcmRef, sizex& pcmRelX, sizex& minBLEPIndex) const {
        auto const pcmMark = getPcmMark(naiveCursor);
        // If pcmCursor is 1 too big due to rounding error, we simply skip _minBLEPs[0] which is close to zero:
        auto const pcmCursor = DSSI::cursor(ceil(pcmMark));
        if (pcmCursor < pcmRef) {
            logRoundingError(naiveCursor, pcmRef, pcmCursor);
            assert(false);
        }
        pcmRelX = sizex(pcmCursor - pcmRef);
        auto const distance = double(pcmCursor) - pcmMark;
        minBLEPIndex = sizex(round(distance * _minBLEPCount));
    }

    void logRoundingError(DSSI::cursor naiveCursor, DSSI::cursor pcmRef, DSSI::cursor pcmCursor) const;

#ifdef CARSTAIRS_TEST
public:
#endif

    inline double getPcmMark(DSSI::cursor naiveCursor) const {
        return double(naiveCursor) * _ratio;
    }

    Paster(double ratio, View<float> const minBLEPs, sizex minBLEPCount)
            : _ratio(ratio), _minBLEPs(minBLEPs), _minBLEPCount(minBLEPCount) {
    }

    inline sizex minBLEPSize(sizex minBLEPIndex) const {
        return (_minBLEPs.limit() - minBLEPIndex + _minBLEPCount - 1) / _minBLEPCount;
    }

public:

    Paster(MinBLEPs const& minBLEPs)
            : Paster(minBLEPs._ratio, minBLEPs._minBLEPs, minBLEPs._minBLEPCount) {
    }

    DSSI::cursor getNaiveCursor(DSSI::cursor pcmCursor) const {
        // First naive cursor that doesn't affect pcm cursor values prior to that given:
        return DSSI::cursor(floor(double(pcmCursor - 1) / _ratio) + 1);
    }

    sizex pcmCountWithOverflow(DSSI::cursor naiveX, DSSI::cursor pcmRef) const {
        sizex pcmRelX, minBLEPIndex;
        pastePrepare(naiveX, pcmRef, pcmRelX, minBLEPIndex);
        return pcmRelX + minBLEPSize(minBLEPIndex);
    }

    void pasteMulti(View<float> derivative, DSSI::cursor naiveRef, DSSI::cursor pcmRef, View<float> pcmBuf, sizex dcCursor, float dc) const {
        pcmBuf.zero(dcCursor, pcmBuf.limit());
        auto const ampCount = derivative.limit();
        auto ampPtr = derivative.begin();
        auto const lim = _minBLEPs.limit(), step = _minBLEPCount;
        auto const srcPtr = _minBLEPs.begin();
        auto const pcmEnd = pcmBuf.end();
        auto const pcmBegin = const_cast<float *>(pcmBuf.begin());
        auto dcPtr = pcmBegin + dcCursor;
        for (sizex i = 0; i < ampCount; ++i) {
            auto const amp = *ampPtr++;
            if (amp) {
                sizex pcmRelX, minBLEPIndex;
                pastePrepare(naiveRef + i, pcmRef, pcmRelX, minBLEPIndex);
                auto pcmPtr = pcmBegin + pcmRelX;
                assert(pcmPtr + minBLEPSize(minBLEPIndex) <= pcmEnd); // Bounds check. TODO: Do this outside loop.
                // The target must be big enough for a minBLEP at maximum pcmX:
                for (auto k = minBLEPIndex; k < lim; k += step) {
                    *pcmPtr++ += amp * srcPtr[k];
                }
                while (dcPtr < pcmPtr) {
                    *dcPtr++ += dc;
                }
                dc += amp;
            }
        }
        while (dcPtr < pcmEnd) {
            *dcPtr++ += dc;
        }
    }

};
