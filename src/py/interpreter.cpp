#include "interpreter.h"

#include <python3.4m/Python.h>
#include <cassert>

#include "../util/util.h"

namespace {
Log const LOG(__FILE__);
}

Interpreter::Interpreter(Python const& python) {
    debug("Creating new sub-interpreter.");
    PyThreadState * const main = python;
    PyEval_AcquireThread(main);
    _state = Py_NewInterpreter();
    assert(_state);
    assert(main != _state);
    assert(PyThreadState_Get() == _state);
    PyEval_ReleaseThread(_state);
    debug("Created: %p", _state);
}

Interpreter& Interpreter::operator=(Python const& python) {
    REFRESH(Interpreter, python);
}

void Interpreter::runTask(std::function<void()> const& task) const {
    PyEval_AcquireThread(_state);
    task();
    PyEval_ReleaseThread(_state);
}

Interpreter::~Interpreter() {
    if (_state) {
        debug("Ending sub-interpreter: %p", _state);
        PyEval_AcquireThread(_state);
        Py_EndInterpreter(_state);
        PyEval_ReleaseLock();
    }
}
