#pragma once

#include "config.h"
#include "dssi/plugin.h"
#include "util/util.h"

class State;

class Program {

public:

    virtual ~Program() {
    }

    virtual float rate() const = 0;

    virtual void fire(int noteFrame, int offFrameOrNeg, State&) const = 0;

};

class State {

    static Bounds<int> const TP_BOUNDS, NP_BOUNDS, EP_BOUNDS, LEVEL4_BOUNDS;

    Config const& _config;

    DSSI::cursor _onOrMax, _offOrMax;

    int _midiNote = 0, _velocity = 0;

    int _programEventIndex; // Only valid when _onOrMax isn't MAX.

    int _offEventIndex; // Only valid when _offOrMax isn't MAX.

#ifdef CARSTAIRS_TEST
public:
#endif

    int _TP = TP_BOUNDS._min, _NP = NP_BOUNDS._min, _EP = EP_BOUNDS._min;

    int _level4 = LEVEL4_BOUNDS._min;

    bool _toneFlag, _noiseFlag, _levelMode;

public:

    State(Config const&);

    void reset();

    void noteOn(DSSI::cursor cursor, int midiNote, int velocity);

    void noteOff(DSSI::cursor cursor, int midiNote);

    int const& TP() const {
        return _TP;
    }

    int const& NP() const {
        return _NP;
    }

    int const& EP() const {
        return _EP;
    }

    DSSI::cursor onOrMax() const {
        return _onOrMax;
    }

    int programEventIndex() const {
        return _programEventIndex;
    }

    void fire(Program const& program) {
        program.fire(_programEventIndex, DSSI::CURSOR_MAX != _offOrMax ? _programEventIndex - _offEventIndex : -1, *this);
        ++_programEventIndex;
    }

    void setTP(int TP) {
        _TP = TP_BOUNDS.clamp(TP);
    }

    void setNP(int NP) {
        _NP = NP_BOUNDS.clamp(NP);
    }

    void setEP(int EP) {
        _EP = EP_BOUNDS.clamp(EP);
    }

    void setLevel4(int level4) {
        _level4 = LEVEL4_BOUNDS.clamp(level4);
    }

    int level5() const {
        return _level4 * 2 + 1; // Observe 4-bit 0 is 5-bit 1.
    }

    bool const& toneFlag() const {
        return _toneFlag;
    }

    bool const& noiseFlag() const {
        return _noiseFlag;
    }

    bool const& levelMode() const {
        return _levelMode;
    }

    void setToneFlag(bool b) {
        _toneFlag = b;
    }

    void setNoiseFlag(bool b) {
        _noiseFlag = b;
    }

    void setLevelMode(bool b) {
        _levelMode = b;
    }

};
