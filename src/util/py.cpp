#include "py.h"

#include <python3.4m/Python.h>

#include "util.h"

static Log const LOG(__FILE__);

void PyRef::xdecref() const {
    trace("XDECREF: %p", _ptr);
    Py_XDECREF(_ptr);
}
