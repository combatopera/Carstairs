#pragma once

#include <ladspa.h>

#include "../buf.h"
#include "util.h"

class MinBLEPs {

public:

    cursor_t getMinNaiveN(cursor_t naiveX, cursor_t pcmCount);

    void paste(cursor_t naiveX, View<int> naiveBuf, View<LADSPA_Data> pcmBuf);

};
