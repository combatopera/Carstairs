#pragma once

#include <ladspa.h>

#include "../config.h"
#include "../node.h"
#include "../state.h"
#include "../util/minbleps.h"
#include "tone.h"

class PCM: public Node<LADSPA_Data> {

    MinBLEPs const _minBLEPs;

    Tone * const _tone;

    void resetImpl();

    void renderImpl();

public:

    PCM(Config const *, State *, Tone *);

};
