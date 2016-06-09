#pragma once

#include <ctime>
#include <memory>
#include <thread>

#include "config.h"
#include "dssi/plugin.h"
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

    void fire(int noteFrame, int offFrameOrNeg, State& state) const {
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

    ProgramInfo const& _info;

    float _rate;

public:

    ProgramImpl(Config const&, Python const&, ProgramInfo const&);

    operator bool() const {
        return _rate;
    }

    float rate() const {
        return _rate;
    }

    void fire(int noteFrame, int offFrameOrNeg, State& state) const;

};

class Loader {

    Python const& _python;

    std::shared_ptr<Program const> * const _programs;

    std::time_t * const _marks;

    ProgramInfos const& _programInfos;

    bool _flag;

    std::thread _thread;

    void poll(Config const&);

    bool reload(sizex index) {
        auto const mark = _programInfos[index].lastWriteTime();
        if (mark != _marks[index]) {
            _marks[index] = mark;
            return true;
        }
        return false;
    }

public:

    Loader(Config const&, Python const&, ProgramInfos const&);

    ~Loader();

    std::shared_ptr<Program const>& program(sizex index) const {
        return _programs[index];
    }

};
