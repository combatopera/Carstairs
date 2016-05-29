#include "state.h"

#include <cmath>

Bounds<int> const State::TP_BOUNDS(1, 0xfff);

Bounds<int> const State::LEVEL4_BOUNDS(0, 0xf);

State::State(Config const& config)
        : _config(config), _onOrMax(DSSI::CURSOR_MAX), _offOrMax(DSSI::CURSOR_MAX), //
        _programEventIndex(), _offEventIndex() {
}

void State::reset() {
    _onOrMax = DSSI::CURSOR_MAX;
    _offOrMax = DSSI::CURSOR_MAX;
}

void State::noteOn(DSSI::cursor cursor, int midiNote, int velocity) {
    debug("ON %d %d %d", cursor, midiNote, velocity);
    _midiNote = midiNote;
    _onOrMax = cursor;
    _offOrMax = DSSI::CURSOR_MAX;
    _velocity = velocity;
    auto const freq = _config._refFreq * powf(2, float(midiNote - _config._refMidiNote) / float(_config._semitones));
    _TP = TP_BOUNDS.clamp((int) roundf(_config._nominalClock / (16 * freq)));
    _programEventIndex = 0;
}

void State::noteOff(DSSI::cursor cursor, int midiNote) {
    if (DSSI::CURSOR_MAX != _onOrMax && _midiNote == midiNote) {
        debug("OFF %d %d", cursor, midiNote);
        _offOrMax = cursor;
        _offEventIndex = _programEventIndex;
    }
}
