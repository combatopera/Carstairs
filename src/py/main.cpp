#include "main.h"

#include <python3.4m/Python.h>
#include <cassert>

#include "../util/util.h"

namespace {

Log const LOG(__FILE__);

PyThreadState *initPython() {
    debug("Initing Python.");
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
    debug("Closing Python.");
    Py_Finalize();
}
