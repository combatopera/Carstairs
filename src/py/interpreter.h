#pragma once

#include <python3.4m/Python.h>
#include <functional>

class Interpreter {

    PyThreadState *_state;

public:

    Interpreter(PyThreadState * const main);

    Interpreter& operator=(PyThreadState * const main);

    void runTask(std::function<void()> const&) const;

    ~Interpreter();

};
