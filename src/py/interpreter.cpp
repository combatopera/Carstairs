// Copyright 2016 Andrzej Cichocki

// This file is part of Carstairs.
//
// Carstairs is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Carstairs is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Carstairs.  If not, see <http://www.gnu.org/licenses/>.

#include "interpreter.h"

#include <python3.4m/Python.h>
#include <cassert>

#include "../util/util.h"

namespace {
Log const LOG(__FILE__);
}

Interpreter::Interpreter(Config const& config, Module const& module, Python const& python)
        : Interpreter(config._modulesDir, module, python) {
}

Interpreter::Interpreter(boost::filesystem::path const& modulesDir, Module const& module, Python const& python) {
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
        // Assume neither path contains triple single quotes:
        script += "sys.path.append('''";
        script += module.dir().string();
        script += "''')\n";
        script += "sys.path.append('''";
        script += modulesDir.string();
        script += "''')\n";
        execute(script);
    }
    PyEval_ReleaseThread(_state);
    CARSTAIRS_DEBUG("Created: %p", _state);
}

void Interpreter::runTask(std::function<void()> const& task) const {
    PyEval_AcquireThread(_state);
    task();
    PyEval_ReleaseThread(_state);
}

Interpreter::~Interpreter() {
    CARSTAIRS_DEBUG("Ending sub-interpreter: %p", _state);
    PyEval_AcquireThread(_state);
    CARSTAIRS_TRACE("Calling: Py_EndInterpreter(%p)", _state);
    Py_EndInterpreter(_state);
    CARSTAIRS_TRACE("Calling: PyEval_ReleaseLock");
    PyEval_ReleaseLock();
    CARSTAIRS_TRACE("Sub-interpreter ended.");
}
