#pragma once

#include <ladspa.h>

#include "../buf.h"
#include "../config.h"
#include "util.h"

class MinBLEPs {

    int const _scale;

public:

    MinBLEPs(Config const *);

    cursor_t getMinNaiveN(cursor_t naiveX, cursor_t pcmCount) const;

    void paste(cursor_t naiveX, View<float> naiveBuf, View<LADSPA_Data> pcmBuf) const;

};
