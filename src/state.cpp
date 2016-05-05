#include "state.h"

#include "util/util.h"

void State::reset() {
    _noteOn = -1;
    _noteOff = -1;
    _velocity = 0;
}

void State::noteOn(cursor_t cursor, int midiNote, int velocity) {
    _midiNote = midiNote;
    _noteOn = cursor;
    _noteOff = -1;
    _velocity = velocity;
}

void State::noteOff(cursor_t cursor, int midiNote) {
    if (_midiNote == midiNote) {
        _noteOff = cursor;
    }
}
