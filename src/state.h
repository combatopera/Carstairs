#pragma once

#include "util/util.h"

class State {

    int _midiNote, _velocity;

    cursor_t _onOrMax, _offOrMax;

#ifdef UNIT_TEST
public:
#endif

    int _TP;

public:

    void reset();

    void noteOn(cursor_t cursor, int midiNote, int velocity);

    void noteOff(cursor_t cursor, int midiNote);

    int const *TP();

};
