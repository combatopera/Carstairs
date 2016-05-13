#include "state.h"

#include <cmath>

#include "util/util.h"

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
    double freq = REF_FREQ * pow(2, ((midiNote - REF_MIDI_NOTE) / (double) SEMITONES));
    _TP = (int) round(CLOCK / (16 * freq));
}

void State::noteOff(cursor_t cursor, int midiNote) {
    if (CURSOR_MAX != _onOrMax && _midiNote == midiNote) {
        debug("OFF %d %d", cursor, midiNote);
        _offOrMax = cursor;
    }
}

int const *State::TP() {
    return &_TP;
}
