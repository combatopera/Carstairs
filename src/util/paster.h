#pragma once

#include "../dssi/plugin.h"
#include "buf.h"
#include "minbleps.h"

class Paster {

    double const _ratio;

    View<float> const _minBLEPs;

    sizex const _minBLEPCount;

    friend inline void pastePrepare(Paster const&, DSSI::cursor naiveX, DSSI::cursor pcmRef, sizex& pcmRelX, sizex& minBLEPIndex);

#ifdef UNIT_TEST
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

    sizex pcmCountWithOverflow(DSSI::cursor naiveX, DSSI::cursor pcmRef) const;

    void pasteMulti(View<float> derivative, DSSI::cursor naiveRef, DSSI::cursor pcmRef, View<float> pcmBuf, sizex dcCursor, float dc);

};
