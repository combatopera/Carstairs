#pragma once

#include "../node.h"
#include "../state.h"
#include "noise.h"
#include "tone.h"

class Mixer: public Node<int> {

    Tone& _tone;

    Noise& _noise;

    void startImpl() {
    }

    void renderImpl();

public:

    Mixer(State const& state, Tone& tone, Noise& noise)
            : Node("Mixer", state), _tone(tone), _noise(noise) {
    }

};
