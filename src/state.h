#pragma once

#include "util/util.h"

class State {

    static Bounds<int> const TP_BOUNDS;

    cursor_t _onOrMax, _offOrMax;

    int _midiNote, _velocity;

#ifdef UNIT_TEST
public:
#endif

    int _TP = TP_BOUNDS._min;

public:

    void reset();

    void noteOn(cursor_t cursor, int midiNote, int velocity);

    void noteOff(cursor_t cursor, int midiNote);

    int const *TP() const {
        return &_TP;
    }

};
