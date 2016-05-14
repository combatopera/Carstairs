#pragma once

#include <ladspa.h>

#include "../config.h"
#include "../node.h"
#include "../state.h"
#include "../util/minbleps.h"
#include "tone.h"

class PCM: public Node<LADSPA_Data> {

    MinBLEPs _minBLEPs;

    Tone *_tone;

    void resetImpl();

    void renderImpl();

public:

    PCM(Config const *, State *, Tone *);

};
