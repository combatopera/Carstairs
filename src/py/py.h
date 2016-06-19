// Copyright 2016 Andrzej Cichocki

// This file is part of Carstairs.
//
// Carstairs is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Carstairs is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Carstairs.  If not, see <http://www.gnu.org/licenses/>.

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

    void setAttr(char const *name, double value) const {
        PyObject_SetAttrString(_ptr, name, PyRef(PyFloat_FromDouble(value)));
    }

    void setAttr(char const *name, PyRef const& value) const {
        PyObject_SetAttrString(_ptr, name, value);
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
