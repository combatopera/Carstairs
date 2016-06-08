#include "py.h"

#include <python3.4m/Python.h>

#include "../util/util.h"

namespace {
Log const LOG(__FILE__);
}

PyRef& PyRef::operator=(PyObject * const ptr) {
    CARSTAIRS_REFRESH(PyRef, ptr);
}

PyRef& PyRef::operator=(PyRef const& that) {
    Py_XINCREF(that._ptr);
    CARSTAIRS_REFRESH(PyRef, that._ptr);
}

PyRef::~PyRef() {
    if (_ptr) {
        CARSTAIRS_TRACE("DECREF: %p * %zd", _ptr, Py_REFCNT(_ptr));
        Py_DECREF(_ptr);
    }
}
