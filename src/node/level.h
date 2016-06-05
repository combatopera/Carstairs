#pragma once

#include "../config.h"
#include "../node.h"
#include "../state.h"
#include "mixer.h"

class Level: public Node<float> {

    float const _maxAmp;

    Mixer& _mixer;

    void startImpl() {
    }

    void renderImpl();

public:

    Level(Config const& config, State& state, Mixer& mixer)
            : Node("Level", state), _maxAmp(config._maxAmp), _mixer(mixer) {
    }

};
