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

#include "main.h"

#include <Python.h>
#include <cassert>

#include "../util/util.h"

namespace {

Log const LOG(__FILE__);

PyThreadState *initPython() {
    CARSTAIRS_DEBUG("Initing Python.");
    Py_InitializeEx(0);
    auto const main = PyThreadState_Get();
    assert(main);
    PyEval_InitThreads();
    PyEval_ReleaseThread(main);
    return main;
}

}

Python::Python()
    : _main(initPython()) {
}

Python::~Python() {
    PyEval_AcquireThread(_main); // Otherwise Py_Finalize crashes.
    CARSTAIRS_DEBUG("Closing Python.");
    Py_Finalize();
}
