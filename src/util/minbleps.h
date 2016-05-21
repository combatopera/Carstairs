#pragma once

#include <ladspa.h>
#include <cmath>

#include "../config.h"
#include "buf.h"
#include "util.h"

class MinBLEPs {

    int const _scale;

    int _minBLEPCount;

    int _pcmRate, _naiveRate;

    Buffer<int> _naiveXToPcmX {"_naiveXToPcmX"};

    Buffer<int> _pcmXToMinNaiveX {"_pcmXToMinNaiveX"};

    Buffer<float> _minBLEPs {"_minBLEPs"};

public:

    MinBLEPs(Config const *);

    DSSI::cursor pcmXToNaiveX(DSSI::cursor pcmX) const {
        // First naiveX that doesn't affect pcmX prior to that given:
        return DSSI::cursor(floor(double(pcmX - 1) / _pcmRate * _naiveRate) + 1);
    }

    void pasteOne(DSSI::cursor naiveX, float amp, DSSI::cursor pcmRef, View<float> target) const {
        auto pcmMark = double(naiveX) / _naiveRate * _pcmRate;
        // If pcmX is 1 too big due to rounding error, we simply skip _minBLEPs[0] which is close to zero:
        auto pcmX = DSSI::cursor(ceil(pcmMark));
        auto distance = double(pcmX) - pcmMark;
        auto k = unsigned(round(distance * _minBLEPCount));
        auto targetPtr = const_cast<float *>(target.begin() + (pcmX - pcmRef));
        // The target must be big enough for a minBLEP at maximum pcmX:
        for (auto lim = _minBLEPs.limit(); k < lim; k += _minBLEPCount) {
            *targetPtr++ += amp * _minBLEPs.at(k);
        }
        for (auto end = target.end(); targetPtr != end;) {
            *targetPtr++ += amp;
        }
    }

    void paste(DSSI::cursor naiveX, View<float> naiveBuf, View<LADSPA_Data> pcmBuf) const;

    void paste2(DSSI::cursor naiveX, View<float> naiveBuf, View<LADSPA_Data> pcmBuf) const;

};
