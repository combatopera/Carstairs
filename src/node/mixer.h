#pragma once

#include "../node.h"
#include "../state.h"
#include "tone.h"

class Mixer: public Node<int> {

    Tone& _tone;

    void startImpl() {
    }

    void renderImpl();

public:

    Mixer(State const& state, Tone& tone)
            : Node("Mixer", state), _tone(tone) {
    }

};
