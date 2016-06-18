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

    sizex pcmCountWithOverflow(DSSI::cursor naiveX, DSSI::cursor pcmRef) const;

    void pasteMulti(View<float> derivative, DSSI::cursor naiveRef, DSSI::cursor pcmRef, View<float> pcmBuf, sizex dcCursor, float dc);

};
