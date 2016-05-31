#include "program.h"

#include <boost/filesystem/operations.hpp>
#include <python3.4m/Python.h>
#include <cassert>

#include "util/util.h"

namespace {

Log const LOG(__FILE__);

Python const PYTHON;

PyThreadState *initPython() {
    debug("Initing Python.");
    Py_InitializeEx(0);
    auto const parent = PyThreadState_Get();
    assert(parent);
    PyEval_InitThreads();
    PyEval_ReleaseThread(parent);
    return parent;
}

}

void Program::newInterpreter() {
    debug("Creating new sub-interpreter.");
    PyEval_AcquireThread(PYTHON._parent);
    _interpreter = Py_NewInterpreter();
    assert(_interpreter);
    assert(PYTHON._parent != _interpreter);
    assert(PyThreadState_Get() == _interpreter);
    PyEval_ReleaseThread(_interpreter);
}

void Program::endInterpreter() {
    debug("Ending sub-interpreter.");
    PyEval_AcquireThread(_interpreter);
    _module = 0; // Probably wise to destroy before its owner interpreter.
    Py_EndInterpreter(_interpreter);
    PyEval_ReleaseLock();
}

Python::Python()
        : _parent(initPython()) {
}

Program::Program(Config const& config)
        : _moduleName(config._programModule), _mark(-1) {
    newInterpreter();
    debug("Loading module: %s", _moduleName);
    PyEval_AcquireThread(_interpreter);
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
    PyEval_ReleaseThread(_interpreter);
}

void Program::refresh() {
    if (!_path.empty()) {
        auto const mark = boost::filesystem::last_write_time(_path);
        if (mark != _mark) {
            _mark = mark;
            endInterpreter();
            newInterpreter();
            debug("Reloading module: %s", _moduleName);
            PyEval_AcquireThread(_interpreter);
            _module = PyImport_ImportModule(_moduleName);
            if (_module) {
                _rate = _module.getAttr("rate").numberToFloatOr(DEFAULT_RATE);
                debug("Program rate: %.3f", _rate);
            }
            else {
                debug("Failed to reload module.");
            }
            PyEval_ReleaseThread(_interpreter);
        }
    }
}

Program::~Program() {
    endInterpreter();
}

Python::~Python() {
    PyEval_AcquireThread(_parent); // Otherwise Py_Finalize crashes.
    debug("Closing Python.");
    Py_Finalize();
}

void Program::fire(int noteFrame, int offFrameOrNeg, State& state) const {
    if (!noteFrame) {
        state.setLevel4(13); // Use half the available amp.
    }
    if (_module) {
        PyEval_AcquireThread(_interpreter);
        if (offFrameOrNeg < 0) {
            _module.getAttr("on").callVoid("(i)", noteFrame);
        }
        else {
            _module.getAttr("off").callVoid("ii", noteFrame, offFrameOrNeg);
        }
        auto const chan = _module.getAttr("chan");
        if (chan) {
            auto const level = chan.getAttr("level");
            if (level) {
                state.setLevel4(level.numberRoundToInt());
            }
        }
        PyEval_ReleaseThread(_interpreter);
    }
}
