#pragma once

#include <python3.4m/Python.h>
#include <cmath>
#include <cstdarg>

class PyRef {

    PyObject *_ptr;

public:

    PyRef(PyObject * const ptr = 0)
            : _ptr(ptr) {
    }

    PyRef& operator=(PyObject * const ptr);

    PyRef& operator=(PyRef const& that);

    ~PyRef();

    explicit operator bool() const {
        return _ptr;
    }

    PyRef getAttr(char const *name) const {
        return PyObject_GetAttrString(_ptr, name);
    }

    void setAttr(char const *name, long value) const {
        PyObject_SetAttrString(_ptr, name, PyRef(PyLong_FromLong(value)));
    }

    float numberToFloat() const {
        return float(PyFloat_AsDouble(_ptr));
    }

    int numberRoundToInt() const {
        return int(round(PyFloat_AsDouble(_ptr)));
    }

    bool boolValue() const {
        return PyObject_IsTrue(_ptr);
    }

    void callVoid(char const *format, ...) const {
        va_list ap;
        va_start(ap, format);
        PyRef args = Py_VaBuildValue(format, ap);
        va_end(ap);
        PyRef none = PyEval_CallObject(_ptr, args);
        if (!none) {
            PyErr_Print();
        }
    }

    PyRef toPathBytes() const {
        return PyUnicode_EncodeFSDefault(_ptr);
    }

    char const *unwrapBytes() const {
        return PyBytes_AsString(_ptr);
    }

    operator PyObject *() const {
        return _ptr;
    }

};
