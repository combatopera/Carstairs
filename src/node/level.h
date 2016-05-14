#pragma once

#include "../node.h"
#include "../state.h"
#include "tone.h"

class Level: public Node<float> {

    Tone * const _tone;

    void resetImpl();

    void renderImpl();

public:

    Level(State *state, Tone * tone)
            : Node("Level", state), _tone(tone) {
    }

};
