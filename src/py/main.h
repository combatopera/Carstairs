#pragma once

#include <python3.4m/Python.h>

class Python {

    PyThreadState * const _main;

public:

    Python();

    ~Python();

    operator PyThreadState *() const {
        return _main;
    }

};
