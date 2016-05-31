#pragma once

#include <boost/filesystem/path.hpp>
#include <ctime>
#include <memory>

#include "config.h"
#include "py/interpreter.h"
#include "py/py.h"
#include "state.h"

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

    PyRef _module;

    float _rate;

public:

    ProgramImpl(char const *name);

    operator bool() const {
        return _module;
    }

    float rate() const {
        return _rate;
    }

    void fire(int noteFrame, int offFrameOrNeg, State& state) const;

};

class Loader {

    std::shared_ptr<Program> _programHolder {new DefaultProgram};

    char const * const _moduleName;

    std::time_t _mark;

    boost::filesystem::path _path;

public:

    Loader(Config const&);

    void refresh();

    Program& currentProgram() const {
        return *_programHolder.get();
    }

};
