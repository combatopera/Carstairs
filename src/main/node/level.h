// Copyright 2016 Andrzej Cichocki

// This file is part of Carstairs.
//
// Carstairs is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Carstairs is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Carstairs.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <array>
#include <cmath>

#include "../config.h"
#include "../node.h"
#include "../state.h"
#include "../util/buf.h"
#include "env.h"
#include "mixer.h"

class Level: public Node<float> {

    std::array<float, 32> _level5ToAmp;

    bool const& _levelMode;

    Mixer& _mixer;

    Env& _env;

    void startImpl() {
        // Nothing to be done.
    }

    void renderImpl() {
        auto const n = _buf.limit();
        auto const newCursor = cursor() + n;
        auto const binary = _mixer.render(newCursor);
        auto binPtr = binary.begin();
        auto outPtr = const_cast<float *>(_buf.begin());
        if (_levelMode) {
            auto const env = _env.render(newCursor);
            auto level5Ptr = env.begin();
            for (auto i = n - 1; SIZEX_NEG != i; --i) {
                *outPtr++ = float(*binPtr++ * 2 - 1) * _level5ToAmp[*level5Ptr++];
            }
        }
        else {
            auto const amp = _level5ToAmp[_state.level5()];
            for (auto i = n - 1; SIZEX_NEG != i; --i) {
                *outPtr++ = float(*binPtr++ * 2 - 1) * amp;
            }
        }
    }

public:

    Level(Config const& config, State const& state, Mixer& mixer, Env& env)
        : Node("Level", state), _levelMode(state.levelMode()), _mixer(mixer), _env(env) {
        auto const maxLevel5 = sizex(_level5ToAmp.size() - 1);
        for (auto level5 = maxLevel5; SIZEX_NEG != level5; --level5) {
            _level5ToAmp[level5] = powf(2, float(maxLevel5 - level5) / -4) * config._maxAmp;
        }
    }

};
