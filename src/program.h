#pragma once

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

public:

    ~DefaultProgram();

    float rate() const {
        return 50;
    }

    void fire(int noteFrame, int offFrameOrNeg, State& state, EnvShape&) const {
        if (!noteFrame) {
            state.setLevel4(13);
            state.setToneFlag(true);
        }
        if (!offFrameOrNeg) {
            state.setToneFlag(false);
        }
    }

};

class ProgramImpl: public Interpreter, public Program {

    int const _refMidiNote, _semitones;

    float const _refFreq;

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

    std::shared_ptr<Program const> * const _programs;

    std::time_t * const _marks;

    ProgramInfos const& _programInfos;

    bool _flag;

    std::thread _thread;

    void poll(Config const&, Module const&);

    bool reload(sizex index) {
        auto const& info = _programInfos[index];
        if (info.isReal()) {
            auto const mark = static_cast<ProgramInfoImpl const *>(&info)->lastWriteTime();
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

    std::shared_ptr<Program const>& program(sizex index) const {
        return _programs[index];
    }

};
