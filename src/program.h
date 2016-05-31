#pragma once

#include <boost/filesystem/path.hpp>
#include <ctime>
#include <thread>

#include "config.h"
#include "py/interpreter.h"
#include "py/py.h"
#include "state.h"

class Program: public Fire {

    static float constexpr DEFAULT_RATE = 50;

    char const * const _moduleName;

    Interpreter _interpreter;

    std::time_t _mark;

    boost::filesystem::path _path;

    std::thread _thread;

    bool _pollEnabled;

    PyRef _module;

    float _rate = DEFAULT_RATE; // In case initial load fails.

public:

    Program(Config const&);

    ~Program();

    void refresh();

    float rate() const {
        return _rate;
    }

    void fire(int, int, State&) const;

};
