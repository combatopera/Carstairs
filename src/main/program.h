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

#include <cmath>
#include <ctime>
#include <memory>
#include <thread>

#include "config.h"
#include "dssi/plugin.h"
#include "module.h"
#include "py/interpreter.h"
#include "py/main.h"
#include "state.h"
#include "util/buf.h"

class DefaultProgram: public Program {

    float const _scale;

public:

    DefaultProgram(Config const& config)
            : Program(config), _scale(config._nominalClock / float(config.YM2149_ATOM_SIZE * 2)) {
    }

    ~DefaultProgram();

    float rate() const {
        return 50;
    }

    void fire(int noteFrame, int offFrameOrNeg, State& state, EnvShape&) const {
        if (!noteFrame) {
            state.setToneFlag(true);
            state.setNoiseFlag(false);
            state.setLevelMode(false);
            state.setLevel4(13);
            state.setTP(int(roundf(_scale / freq(state.midiNote()))));
        }
        if (!offFrameOrNeg) {
            state.setToneFlag(false);
        }
    }

};

class ProgramImpl: public Interpreter, public Program {

    ProgramInfo const& _info;

    float _rate;

public:

    ProgramImpl(Config const&, Module const&, Python const&, ProgramInfo const&);

    operator bool() const {
        return _rate;
    }

    float rate() const {
        return _rate;
    }

    void fire(int noteFrame, int offFrameOrNeg, State&, EnvShape&) const;

};

class Loader {

    Python const& _python;

    std::shared_ptr<Program const> const _defaultProgram;

    std::shared_ptr<Program const> * const _programs;

    std::time_t * const _marks;

    ProgramInfos const& _programInfos;

    bool _flag;

    std::thread _thread;

    void poll(Config const&, Module const&);

    bool reload(sizex index) {
        auto const& info = _programInfos[index];
        if (info.isReal()) {
            auto const mark = static_cast<ProgramInfoImpl const *>(&info)->lastWriteTime(_marks[index]);
            if (mark != _marks[index]) {
                _marks[index] = mark;
                return true;
            }
        }
        return false;
    }

public:

    Loader(Config const&, Module const&, Python const&, ProgramInfos const&);

    ~Loader();

    std::shared_ptr<Program const> const& program(sizex index) const {
        if (SIZEX_NEG == index) {
            return _defaultProgram;
        }
        else {
            return _programs[index];
        }
    }

};
