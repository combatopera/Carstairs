#include "state.h"

#include <cmath>

#include "config.h"
#include "util/util.h"

Bounds<int> const State::TP_BOUNDS(1, 0xfff);

State::State(Config const *config)
        : _config(config), _onOrMax(CURSOR_MAX), _offOrMax(CURSOR_MAX) {
}

void State::reset() {
    _onOrMax = CURSOR_MAX;
    _offOrMax = CURSOR_MAX;
}

int const REF_MIDI_NOTE = 69, REF_FREQ = 440, SEMITONES = 12, CLOCK = 2000000;

void State::noteOn(cursor_t cursor, int midiNote, int velocity) {
    debug("ON %d %d %d", cursor, midiNote, velocity);
    _midiNote = midiNote;
    _onOrMax = cursor;
    _offOrMax = CURSOR_MAX;
    _velocity = velocity;
    float freq = REF_FREQ * powf(2, float(midiNote - REF_MIDI_NOTE) / float(SEMITONES));
    _TP = TP_BOUNDS.clamp((int) roundf(CLOCK / (16 * freq)));
}

void State::noteOff(cursor_t cursor, int midiNote) {
    if (CURSOR_MAX != _onOrMax && _midiNote == midiNote) {
        debug("OFF %d %d", cursor, midiNote);
        _offOrMax = cursor;
    }
}
