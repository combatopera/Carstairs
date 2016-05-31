#include "interpreter.h"

#include <python3.4m/Python.h>
#include <cassert>

#include "../util/util.h"

namespace {
Log const LOG(__FILE__);
}

Interpreter::Interpreter(PyThreadState * const main) {
    debug("Creating new sub-interpreter.");
    PyEval_AcquireThread(main);
    _state = Py_NewInterpreter();
    assert(_state);
    assert(main != _state);
    assert(PyThreadState_Get() == _state);
    PyEval_ReleaseThread(_state);
    debug("Created: %p", _state);
}

Interpreter& Interpreter::operator=(PyThreadState * const main) {
    this->~Interpreter();
    new (this) Interpreter(main);
    return *this;
}

void Interpreter::runTask(std::function<void()> const& task) const {
    PyEval_AcquireThread(_state);
    task();
    PyEval_ReleaseThread(_state);
}

Interpreter::~Interpreter() {
    debug("Ending sub-interpreter: %p", _state);
    PyEval_AcquireThread(_state);
    Py_EndInterpreter(_state);
    PyEval_ReleaseLock();
}
