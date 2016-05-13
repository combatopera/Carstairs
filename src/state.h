#pragma once

#include "config.h"
#include "util/util.h"

class State {

    static Bounds<int> const TP_BOUNDS;

    Config const * const _config;

    cursor_t _onOrMax, _offOrMax;

    int _midiNote = 0, _velocity = 0;

#ifdef UNIT_TEST
public:
#endif

    int _TP = TP_BOUNDS._min;

public:

    State(Config const *);

    void reset();

    void noteOn(cursor_t cursor, int midiNote, int velocity);

    void noteOff(cursor_t cursor, int midiNote);

    int const *TP() const {
        return &_TP;
    }

};
