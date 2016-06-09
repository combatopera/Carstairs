#pragma once

#include <cmath>

#include "../config.h"
#include "../node.h"
#include "../state.h"
#include "../util/buf.h"
#include "mixer.h"

class Level: public Node<float> {

    float const _maxAmp;

    Mixer& _mixer;

    void startImpl() {
        // Nothing to be done.
    }

    void renderImpl() {
        auto const amp = powf(2, (float(_state.level5()) - 31) / 4) * _maxAmp;
        auto const binary = _mixer.render(cursor() + _buf.limit());
        for (auto i = _buf.limit() - 1; SIZEX_NEG != i; --i) {
            _buf.put(i, float(binary.at(i) * 2 - 1) * amp);
        }
    }

public:

    Level(Config const& config, State& state, Mixer& mixer)
            : Node("Level", state), _maxAmp(config._maxAmp), _mixer(mixer) {
    }

};
