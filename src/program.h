#pragma once

#include <python3.4m/Python.h>
#include <cassert>

#include "config.h"
#include "state.h"
#include "util/util.h"

class Interpreter {

    char const * const _moduleName;

    PyThreadState *_main, *_current;

    PyObject *_module;

    void load() {
        debug("Creating new sub-interpreter.");
        _current = Py_NewInterpreter();
        assert(_current);
        assert(_main != _current);
        assert(PyThreadState_Get() == _current);
        debug("Loading module: %s", _moduleName);
        _module = PyImport_ImportModule(_moduleName);
        if (!_module) {
            debug("Failed to load: %s", _moduleName);
        }
    }

    void unload() {
        debug("Ending sub-interpreter.");
        Py_XDECREF(_module);
        _module = 0;
        Py_EndInterpreter(_current);
    }

public:

    Interpreter(Config const& config)
            : _moduleName(config._programModule) {
        debug("Initing Python.");
        Py_InitializeEx(0);
        _main = PyThreadState_Get();
        assert(_main);
        load();
    }

    void reload() {
        unload();
        load();
    }

    ~Interpreter() {
        unload();
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
