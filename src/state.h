#pragma once

#include "config.h"
#include "dssi/plugin.h"
#include "util/util.h"

class State;

class Fire {

public:

    void fire(int noteFrame, int offFrameOrNeg, State&) const;

};

class State {

    static Bounds<int> const TP_BOUNDS, LEVEL4_BOUNDS;

    Config const& _config;

    DSSI::cursor _onOrMax, _offOrMax;

    int _midiNote = 0, _velocity = 0;

    int _programEventIndex; // Only valid when onOrMax() isn't MAX.

    int _offEventIndex; // Only valid when offOrMax() isn't MAX.

#ifdef DIZZYM_UNIT_TEST
public:
#endif

    int _TP = TP_BOUNDS._min, _level4 = LEVEL4_BOUNDS._min;

public:

    State(Config const&);

    void reset();

    void noteOn(DSSI::cursor cursor, int midiNote, int velocity);

    void noteOff(DSSI::cursor cursor, int midiNote);

    int TP() const {
        return _TP;
    }

    DSSI::cursor onOrMax() const {
        return _onOrMax;
    }

    DSSI::cursor offOrMax() const {
        return _offOrMax;
    }

    int programEventIndex() const {
        return _programEventIndex;
    }

    void fire(Fire const& program) {
        program.fire(_programEventIndex, DSSI::CURSOR_MAX != _offOrMax ? _programEventIndex - _offEventIndex : -1, *this);
        ++_programEventIndex;
    }

    void setLevel4(int level4) {
        _level4 = LEVEL4_BOUNDS.clamp(level4);
    }

    int level5() const {
        return _level4 * 2 + 1; // Observe 4-bit 0 is 5-bit 1.
    }

};
