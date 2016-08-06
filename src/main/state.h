// Copyright 2016 Andrzej Cichocki

// This file is part of Carstairs.
//
// Carstairs is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Carstairs is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Carstairs.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <cassert>
#include <cmath>

#include "config.h"
#include "dssi/plugin.h"
#include "util/util.h"

class EnvShape {

    static Bounds<int> const SHAPE_BOUNDS;

    int _shapeIndex = SHAPE_BOUNDS._min;

protected:

    int shapeIndex() const {
        return _shapeIndex;
    }

    virtual void shapeChanged() = 0;

public:

    virtual ~EnvShape() {
    }

    void changeShape(int shape) {
        _shapeIndex = SHAPE_BOUNDS.clamp(shape);
        shapeChanged();
    }

    char const *envShapeName() const {
        int s;
        if (_shapeIndex & 0b1000) {
            s = _shapeIndex;
        }
        else {
            s = (_shapeIndex & 0b0100) ? 0b1111 : 0b1001;
        }
        switch (s) {
            case 0b1000:
                return "desc_saw";
            case 0b1001:
                return "desc_hold";
            case 0b1010:
                return "desc_tri";
            case 0b1011:
                return "desc_flip";
            case 0b1100:
                return "asc_saw";
            case 0b1101:
                return "asc_hold";
            case 0b1110:
                return "asc_tri";
            case 0b1111:
                return "asc_flip";
        }
        assert(false);
        return 0;
    }

};

class State;

class Program {

protected:

    int const _refMidiNote, _semitones;

    float const _refFreq;

    float freq(int const midiNote) const {
        return _refFreq * powf(2, float(midiNote - _refMidiNote) / float(_semitones));
    }

public:

    Program(Config const& config)
            : _refMidiNote(config._refMidiNote), _semitones(config._semitones), _refFreq(config._refFreq) {
    }

    virtual ~Program() {
    }

    virtual float rate() const = 0;

    virtual void fire(int noteFrame, int offFrameOrNeg, State&, EnvShape&) const = 0;

};

class State {

    static Log const LOG;

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

    int midiNote() const {
        return _midiNote;
    }

    int velocity() const {
        return _velocity;
    }

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

    void fire(Program const& program, EnvShape& envShape) {
        program.fire(_programEventIndex, DSSI::CURSOR_MAX != _offOrMax ? _programEventIndex - _offEventIndex : -1, *this, envShape);
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
