#pragma once

#include <python3.4m/Python.h>

#include "util.h"

class PyRef {

    PyObject *_ptr;

    void xdecref() const {
        debug("XDECREF: %p", _ptr);
        Py_XDECREF(_ptr);
    }

public:

    PyRef(PyObject *ptr = 0)
            : _ptr(ptr) {
    }

    PyRef& operator=(PyObject * const ptr) {
        xdecref();
        _ptr = ptr;
        return *this;
    }

    ~PyRef() {
        xdecref();
    }

    explicit operator bool() const {
        return _ptr;
    }

    PyRef getAttr(char const *name) const {
        return PyObject_GetAttrString(_ptr, name);
    }

    float toFloatOr(float const value) const {
        return _ptr ? float(PyFloat_AsDouble(_ptr)) : value;
    }

    PyRef toPathBytes() const {
        return PyUnicode_EncodeFSDefault(_ptr);
    }

    char const *unwrapBytes() const {
        return PyBytes_AsString(_ptr);
    }

};
