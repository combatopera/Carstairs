#pragma once

#include <python3.4m/Python.h>
#include <cassert>

#include "config.h"
#include "state.h"
#include "util/py.h"
#include "util/util.h"

class Program: public Fire {

    static float constexpr DEFAULT_RATE = 50;

    char const * const _moduleName;

    PyThreadState *_parent, *_interpreter;

    PyRef _module, _path;

    float _rate = DEFAULT_RATE;

public:

    Program(Config const& config)
            : _moduleName(config._programModule) {
        debug("Initing Python.");
        Py_InitializeEx(0);
        _parent = PyThreadState_Get();
        assert(_parent);
        debug("Creating new sub-interpreter.");
        _interpreter = Py_NewInterpreter();
        assert(_interpreter);
        assert(_parent != _interpreter);
        assert(PyThreadState_Get() == _interpreter);
        debug("Loading module: %s", _moduleName);
        _module = PyImport_ImportModule(_moduleName);
        if (_module) {
            _path = _module.getAttr("__file__").toPathBytes();
            debug("Module path: %s", _path.unwrapBytes());
            _rate = _module.getAttr("rate").toFloatOr(DEFAULT_RATE);
            debug("Program rate: %.3f", _rate);
        }
        else {
            debug("Failed to load: %s", _moduleName);
        }
    }

    float rate() const {
        return _rate;
    }

    void fire(int, int, State&) const;

    ~Program() {
        debug("Ending sub-interpreter.");
        _module = _path = 0;
        Py_EndInterpreter(_interpreter);
        PyThreadState_Swap(_parent); // Otherwise Py_Finalize crashes.
        debug("Closing Python.");
        Py_Finalize();
    }

};
