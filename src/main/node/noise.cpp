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

#include "noise.h"

#include <array>

#include "../util/buf.h"

namespace {

class YM2149Noise: public Shape {

    static int const INITIAL_X = 1;

    static int step(int const mask, int& x) {
        auto const bit = x & 1;
        x >>= 1;
        if (bit) {
            x ^= mask;
        }
        return 1 - bit; // Authentic, see qnoispec.
    }

    class LFSR {

    public:

        int _mask = 0;

        sizex _size = 0;

        LFSR() {
            std::array<int, 2> constexpr YM2149_POSITIVE_DEGREES {17, 14};
            for (auto const positiveDegree : YM2149_POSITIVE_DEGREES) {
                _mask += 1 << (positiveDegree - 1);
            }
            auto x = INITIAL_X;
            do {
                step(_mask, x);
                ++_size;
            } while (INITIAL_X != x);
        }

    };

    YM2149Noise(LFSR const& lfsr)
        : Shape("NOISE", lfsr._size) {
        auto x = INITIAL_X;
        for (auto i = 0u, n = lfsr._size; i < n; ++i) {
            _data.put(i, step(lfsr._mask, x));
        }
    }

public:

    YM2149Noise()
        : YM2149Noise(LFSR()) {
    }

} YM2149_NOISE;

}

Noise::Noise(Config const& config, State const& state)
    : Noise(config, state, YM2149_NOISE) {
}

// Doubling the atom size results in authentic spectrum, see qnoispec:
Noise::Noise(Config const& config, State const& state, Shape const& shape)
    : Osc(config._atomSize * 2, state, "Noise", shape, state.NP(), false) {
}

#ifdef CARSTAIRS_TEST
Shape const& noiseShape() {
    return YM2149_NOISE;
}
#endif
