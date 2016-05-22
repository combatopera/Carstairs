#pragma once

#include "../config.h"
#include "../node.h"
#include "../state.h"
#include "tone.h"

class Level: public Node<float> {

    float const _maxAmp;

    Tone& _tone;

    void resetImpl();

    void renderImpl();

public:

    Level(Config const& config, State& state, Tone& tone)
            : Node("Level", state), _maxAmp(config._maxAmp), _tone(tone) {
    }

};
