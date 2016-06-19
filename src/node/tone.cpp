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

#include "tone.h"

#include "../util/buf.h"

namespace {

class Square: public Shape {

public:

    Square()
            : Shape("SQUARE", 2) {
        _data.put(0, 1);
        _data.put(1, 0);
    }

} SQUARE;

}

Tone::Tone(Config const& config, State const& state)
        : Osc(config._atomSize, state, "Tone", SQUARE, state.TP(), true) {
}
