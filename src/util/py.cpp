#include "py.h"

#include <python3.4m/Python.h>

static Log const LOG(__FILE__);

void PyRef::xdecref() const {
    debug("XDECREF: %p", _ptr);
    Py_XDECREF(_ptr);
}
