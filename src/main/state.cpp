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

#include "state.h"

Log const State::LOG(__FILE__);

Bounds<int> const State::TP_BOUNDS(1, 0xfff);

Bounds<int> const State::NP_BOUNDS(1, 0x1f);

Bounds<int> const State::EP_BOUNDS(1, 0xffff);

Bounds<int> const EnvShape::SHAPE_BOUNDS(0, 0b1111);

Bounds<int> const State::LEVEL4_BOUNDS(0, 0xf);

State::State(Config const& config)
        : _config(config), _onOrMax(DSSI::CURSOR_MAX), _offOrMax(DSSI::CURSOR_MAX), //
        _programEventIndex(), _offEventIndex(), _toneFlag(), _noiseFlag(), _levelMode() {
}

void State::reset() {
    _onOrMax = DSSI::CURSOR_MAX;
    _offOrMax = DSSI::CURSOR_MAX;
}

void State::noteOn(DSSI::cursor cursor, int midiNote, int velocity) {
    CARSTAIRS_DEBUG("ON %lu %d %d", cursor, midiNote, velocity);
    _midiNote = midiNote;
    _onOrMax = cursor;
    _offOrMax = DSSI::CURSOR_MAX;
    _velocity = velocity;
    _programEventIndex = 0;
}

void State::noteOff(DSSI::cursor cursor, int midiNote) {
    if (DSSI::CURSOR_MAX != _onOrMax && _midiNote == midiNote) {
        CARSTAIRS_DEBUG("OFF %lu %d", cursor, midiNote);
        _offOrMax = cursor;
        _offEventIndex = _programEventIndex;
    }
}
