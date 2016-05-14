#pragma once

#include "config.h"
#include "util/util.h"

class State {

    static Bounds<int> const TP_BOUNDS, LEVEL4_BOUNDS;

    Config const * const _config;

    cursor_t _onOrMax, _offOrMax;

    int _midiNote = 0, _velocity = 0;

#ifdef UNIT_TEST
public:
#endif

    int _TP = TP_BOUNDS._min, _level4 = LEVEL4_BOUNDS._max; // TODO: Default level should be _min.

public:

    State(Config const *);

    void reset();

    void noteOn(cursor_t cursor, int midiNote, int velocity);

    void noteOff(cursor_t cursor, int midiNote);

    int const *TP() const {
        return &_TP;
    }

    int level5() const {
        return _level4 * 2 + 1; // Observe 4-bit 0 is 5-bit 1.
    }

};
