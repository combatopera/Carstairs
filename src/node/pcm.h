#pragma once

#include <ladspa.h>

#include "../node.h"
#include "../state.h"
#include "../util/minbleps.h"
#include "tone.h"

class PCM: public Node<LADSPA_Data> {

    Tone _tone;

    MinBLEPs _minBLEPs;

    void renderImpl();

public:

    PCM(State *);

};
