#include "state.h"

namespace {
Log const LOG(__FILE__);
}

Bounds<int> const State::TP_BOUNDS(1, 0xfff);

Bounds<int> const State::NP_BOUNDS(1, 0x1f);

Bounds<int> const State::EP_BOUNDS(1, 0xffff);

Bounds<int> const State::SHAPE_BOUNDS(0, 0b1111);

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
