#pragma once

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <python3.4m/Python.h>
#include <cassert>
#include <ctime>

#include "config.h"
#include "state.h"
#include "util/py.h"
#include "util/util.h"

class Program: public Fire {

    static float constexpr DEFAULT_RATE = 50;

    char const * const _moduleName;

    PyThreadState *_parent, *_interpreter;

    boost::filesystem::path _path;

    std::time_t _mark;

    PyRef _module;

    float _rate = DEFAULT_RATE; // In case initial load fails.

    void newInterpreter() {
        debug("Creating new sub-interpreter.");
        _interpreter = Py_NewInterpreter();
        assert(_interpreter);
        assert(_parent != _interpreter);
        assert(PyThreadState_Get() == _interpreter);
    }

    void endInterpreter() {
        debug("Ending sub-interpreter.");
        _module = 0; // Probably wise to destroy before its owner interpreter.
        Py_EndInterpreter(_interpreter);
    }

public:

    Program(Config const& config)
            : _moduleName(config._programModule), _mark(-1) {
        debug("Initing Python.");
        Py_InitializeEx(0);
        _parent = PyThreadState_Get();
        assert(_parent);
        newInterpreter();
        debug("Loading module: %s", _moduleName);
        PyRef module(PyImport_ImportModule(_moduleName));
        if (module) {
            auto const bytes = module.getAttr("__file__").toPathBytes();
            auto const str = bytes.unwrapBytes();
            debug("Module path: %s", str);
            _path = str;
        }
        else {
            debug("Failed to load module, refresh disabled.");
        }
    }

    void refresh() {
        if (!_path.empty()) {
            auto const mark = boost::filesystem::last_write_time(_path);
            if (mark != _mark) {
                _mark = mark;
                endInterpreter();
                newInterpreter();
                debug("Reloading module: %s", _moduleName);
                _module = PyImport_ImportModule(_moduleName);
                if (_module) {
                    _rate = _module.getAttr("rate").toFloatOr(DEFAULT_RATE);
                    debug("Program rate: %.3f", _rate);
                }
                else {
                    debug("Failed to reload module.");
                }
            }
        }
    }

    float rate() const {
        return _rate;
    }

    void fire(int, int, State&) const;

    ~Program() {
        endInterpreter();
        PyThreadState_Swap(_parent); // Otherwise Py_Finalize crashes.
        debug("Closing Python.");
        Py_Finalize();
    }

};
