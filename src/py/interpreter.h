#pragma once

#include <python3.4m/Python.h>
#include <functional>

#include "main.h"
#include "py.h"

class Interpreter {

    PyThreadState *_state;

public:

    static PyRef import(char const *module) {
        return PyImport_ImportModule(module);
    }

    Interpreter(Python const&);

    Interpreter()
            : _state(0) {
    }

    Interpreter& operator=(Python const&);

    void runTask(std::function<void()> const&) const;

    ~Interpreter();

};
