#pragma once

#include "util/util.h"

class State {

    int _midiNote;

    long _noteOn, _noteOff;

    int _velocity;

public:

    void reset();

    void noteOn(cursor_t cursor, int midiNote, int velocity);

    void noteOff(cursor_t cursor, int midiNote);

};
