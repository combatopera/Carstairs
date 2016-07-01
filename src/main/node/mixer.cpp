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

#include "mixer.h"

#include "../util/buf.h"

void Mixer::renderImpl() {
    auto const tone = _tone.render(cursor() + _buf.limit());
    auto const noise = _noise.render(cursor() + _buf.limit());
    if (_state.toneFlag()) {
        _buf.fill(tone.begin());
        if (_state.noiseFlag()) {
            _buf.AND(noise.begin()); // We use AND as zero is preferred over envelope, see qbmixenv.
        }
    }
    else if (_state.noiseFlag()) {
        _buf.fill(noise.begin());
    }
    else {
        _buf.fill(1); // Fixed and variable levels should work, see qanlgmix and qenvpbuf.
    }
}
