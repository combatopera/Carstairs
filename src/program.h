#pragma once

#include <boost/filesystem/path.hpp>
#include <ctime>

#include "config.h"
#include "py/interpreter.h"
#include "py/py.h"
#include "state.h"

class Program: public Fire {

    static float constexpr DEFAULT_RATE = 50;

    char const * const _moduleName;

    Interpreter _interpreter;

    boost::filesystem::path _path;

    std::time_t _mark;

    PyRef _module;

    float _rate = DEFAULT_RATE; // In case initial load fails.

public:

    Program(Config const&);

    void refresh();

    float rate() const {
        return _rate;
    }

    void fire(int, int, State&) const;

};
