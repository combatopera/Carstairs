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

    cursor_t getMinNaiveN(cursor_t naiveX, cursor_t pcmCount) const;

    cursor_t getMinNaiveN2(cursor_t naiveX, cursor_t pcmCount) const;

    void paste(cursor_t naiveX, View<float> naiveBuf, View<LADSPA_Data> pcmBuf) const;

    void paste2(cursor_t naiveX, View<float> naiveBuf, View<LADSPA_Data> pcmBuf) const;

};
