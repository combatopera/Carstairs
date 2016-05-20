#pragma once

#include <ladspa.h>
#include <cmath>

#include "../config.h"
#include "buf.h"
#include "util.h"

class MinBLEPs {

    int const _scale;

    int _pcmRate, _naiveRate;

    Buffer<int> _naiveXToPcmX {"_naiveXToPcmX"};

    Buffer<int> _pcmXToMinNaiveX {"_pcmXToMinNaiveX"};

public:

    MinBLEPs(Config const *);

    DSSI::cursor pcmXToNaiveX(DSSI::cursor pcmX) const {
        // First naiveX that doesn't affect pcmX prior to that given:
        return DSSI::cursor(floor(double(pcmX - 1) / _pcmRate * _naiveRate) + 1);
    }

    void paste(DSSI::cursor naiveX, View<float> naiveBuf, View<LADSPA_Data> pcmBuf) const;

    void paste2(DSSI::cursor naiveX, View<float> naiveBuf, View<LADSPA_Data> pcmBuf) const;

};
