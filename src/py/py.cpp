#include "py.h"

#include <python3.4m/Python.h>

#include "../util/util.h"

namespace {
Log const LOG(__FILE__);
}

PyRef& PyRef::operator=(PyObject * const ptr) {
    this->~PyRef();
    new (this) PyRef(ptr);
    return *this;
}

PyRef::~PyRef() {
    trace("XDECREF: %p", _ptr);
    Py_XDECREF(_ptr);
}
