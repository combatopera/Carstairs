#include "program.h"

#include <python3.4m/Python.h>

void Program::fire(int noteFrame, int offFrameOrNeg, State& state) const {
    if (!noteFrame) {
        state.setLevel4(13); // Use half the available amp.
    }
    if (_module) {
        if (offFrameOrNeg < 0) {
            PyRef on = _module.getAttr("on");
            PyRef args = Py_BuildValue("(i)", noteFrame);
            Py_XDECREF(PyEval_CallObject(on, args));
        }
        else {
            PyRef off = _module.getAttr("off");
            PyRef args = Py_BuildValue("ii", noteFrame, offFrameOrNeg);
            Py_XDECREF(PyEval_CallObject(off, args));
        }
    }
}
