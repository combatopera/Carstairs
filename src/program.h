#pragma once

#include <python3.4m/Python.h>
#include <cassert>
#include <cstdio>

#include "config.h"
#include "state.h"
#include "util/util.h"

class Interpreter {

    char const * const _programPath;

    PyThreadState *_main, *_current;

    void newInterpreter() {
        debug("Creating new sub-interpreter.");
        _current = Py_NewInterpreter();
        assert(_current);
        assert(PyThreadState_Get() == _current);
    }

    void endInterpreter() {
        debug("Ending sub-interpreter.");
        Py_EndInterpreter(_current);
    }

    void load() {
        newInterpreter();
        debug("Loading: %s", _programPath);
        auto const f = fopen(_programPath, "r");
        if (f) {
            PyRun_SimpleFile(f, _programPath);
            fclose(f);
        }
        else {
            debug("Failed to open: %s", _programPath);
        }
    }

public:

    Interpreter(Config const& config)
            : _programPath(config._programPath) {
        debug("Initing Python.");
        Py_InitializeEx(0);
        PyEval_InitThreads();
        _main = PyThreadState_Get();
        assert(_main);
        load();
    }

    void reload() {
        endInterpreter();
        load();
    }

    ~Interpreter() {
        endInterpreter();
        PyThreadState_Swap(_main); // Otherwise Py_Finalize crashes.
        debug("Closing Python.");
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
