#include "interpreter.h"

#include <python3.4m/Python.h>
#include <cassert>
#include <string>

#include "../util/util.h"

namespace {
Log const LOG(__FILE__);
}

Interpreter::Interpreter(Config const& config, Python const& python)
        : Interpreter(config._modulesDir, python) {
}

Interpreter::Interpreter(char const *modulesDir, Python const& python)
        : _modulesDir(modulesDir) {
    CARSTAIRS_DEBUG("Creating new sub-interpreter.");
    PyThreadState * const main = python;
    PyEval_AcquireThread(main);
    _state = Py_NewInterpreter();
    assert(_state);
    assert(main != _state);
    assert(PyThreadState_Get() == _state);
    {
        std::string script;
        script += "import sys\n";
        script += "sys.path.append('";
        script += modulesDir;
        script += "')\n";
        PyRun_SimpleString(script.c_str());
    }
    PyEval_ReleaseThread(_state);
    CARSTAIRS_DEBUG("Created: %p", _state);
}

Interpreter& Interpreter::operator=(Python const& python) {
    CARSTAIRS_REFRESH(Interpreter, _modulesDir, python);
}

void Interpreter::runTask(std::function<void()> const& task) const {
    PyEval_AcquireThread(_state);
    task();
    PyEval_ReleaseThread(_state);
}

Interpreter::~Interpreter() {
    CARSTAIRS_DEBUG("Ending sub-interpreter: %p", _state);
    PyEval_AcquireThread(_state);
    CARSTAIRS_DEBUG("Calling: Py_EndInterpreter(%p)", _state);
    Py_EndInterpreter(_state);
    CARSTAIRS_DEBUG("Calling: PyEval_ReleaseLock");
    PyEval_ReleaseLock();
    CARSTAIRS_DEBUG("Sub-interpreter ended.");
}
