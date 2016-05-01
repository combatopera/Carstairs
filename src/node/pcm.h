#pragma once

#include <ladspa.h>

#include "../node.h"
#include "tone.h"

class PCM: public Node<LADSPA_Data> {

    Tone _tone;

    void renderImpl(unsigned long n);

};
