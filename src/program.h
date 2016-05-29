#pragma once

#include <python3.4m/Python.h>
#include <cstdio>

#include "config.h"
#include "state.h"
#include "util/util.h"

class Interpreter {

public:

    Interpreter(Config const& config) {
        Py_InitializeEx(0);
        debug("Loading: %s", config._programPath);
        auto const f = fopen(config._programPath, "r");
        if (f) {
            PyRun_SimpleFile(f, config._programPath);
            fclose(f);
        }
        else {
            debug("Failed to open: %s", config._programPath);
        }
    }

    ~Interpreter() {
        Py_Finalize();
    }

};

class Program: public Fire {

    Interpreter const _interpreter;

    float const _rate;

public:

    Program(Config const& config, float rate)
            : _interpreter {config}, _rate(rate) {
    }

    float rate() const {
        return _rate;
    }

    void fire(int, int, State&) const;

};
