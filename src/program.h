#pragma once

#include <boost/filesystem/path.hpp>
#include <ctime>
#include <memory>
#include <thread>

#include "config.h"
#include "py/interpreter.h"
#include "py/main.h"
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

    char const * const _name;

    float _rate;

public:

    ProgramImpl(Config const&, Python const&, char const *name);

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

    std::shared_ptr<Program> _nextProgram {new DefaultProgram}, _currentProgram;

    char const * const _moduleName;

    std::time_t _mark;

    boost::filesystem::path _path;

    bool _flag;

    std::thread _thread;

    void poll(Config const&);

public:

    Loader(Config const&, Python const&);

    ~Loader();

    void refresh() {
        _currentProgram = _nextProgram;
    }

    Program& currentProgram() const {
        return *_currentProgram.get();
    }

};
