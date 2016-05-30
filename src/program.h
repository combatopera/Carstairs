#pragma once

#include <boost/filesystem/path.hpp>
#include <python3.4m/Python.h>
#include <ctime>

#include "config.h"
#include "state.h"
#include "util/py.h"

class Program: public Fire {

    static float constexpr DEFAULT_RATE = 50;

    char const * const _moduleName;

    PyThreadState * const _parent;

    PyThreadState *_interpreter;

    boost::filesystem::path _path;

    std::time_t _mark;

    PyRef _module;

    float _rate = DEFAULT_RATE; // In case initial load fails.

    void newInterpreter();

    void endInterpreter();

public:

    Program(Config const&);

    void refresh();

    float rate() const {
        return _rate;
    }

    void fire(int, int, State&) const;

    ~Program();

};
