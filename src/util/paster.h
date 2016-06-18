#pragma once

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

    friend inline double getPcmMark(Paster const& paster, DSSI::cursor naiveCursor);

    friend inline void pastePrepare(Paster const&, DSSI::cursor naiveX, DSSI::cursor pcmRef, sizex& pcmRelX, sizex& minBLEPIndex);

#ifdef CARSTAIRS_TEST
public:
#endif

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
