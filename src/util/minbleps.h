#pragma once

#include <ladspa.h>

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

    DSSI::cursor getMinNaiveN(DSSI::cursor naiveX, DSSI::cursor pcmCount) const;

    DSSI::cursor getMinNaiveN2(DSSI::cursor naiveX, DSSI::cursor pcmCount) const;

    void paste(DSSI::cursor naiveX, View<float> naiveBuf, View<LADSPA_Data> pcmBuf) const;

    void paste2(DSSI::cursor naiveX, View<float> naiveBuf, View<LADSPA_Data> pcmBuf) const;

};
