#pragma once

#include <python3.4m/Python.h>
#include <functional>

#include "py.h"

class Interpreter {

    PyThreadState *_state;

public:

    static PyRef import(char const *module) {
        return PyImport_ImportModule(module);
    }

    Interpreter(PyThreadState * const main);

    Interpreter& operator=(PyThreadState * const main);

    void runTask(std::function<void()> const&) const;

    ~Interpreter();

};
