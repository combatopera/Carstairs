#pragma once

#include <memory>
#include <thread>

#include "config.h"
#include "dssi/plugin.h"
#include "py/interpreter.h"
#include "py/main.h"
#include "state.h"
#include "util/buf.h"

class DefaultProgram: public Program {

    float rate() const {
        return 50;
    }

    void fire(int noteFrame, int offFrameOrNeg, State& state) const {
        if (offFrameOrNeg < 0) {
            state.setLevel4(13);
        }
        else {
            state.setLevel4(0);
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

    ProgramInfos const& _programInfos;

    bool _flag;

    std::thread _thread;

    void poll(Config const&);

public:

    Loader(Config const&, Python const&, ProgramInfos const&);

    ~Loader();

    Program const& program(sizex index) const {
        return *_programs[index];
    }

};
