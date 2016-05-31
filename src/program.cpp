#include "program.h"

#include <boost/filesystem/operations.hpp>
#include <python3.4m/Python.h>
#include <cassert>
#include <functional>

#include "util/util.h"

namespace {

Log const LOG(__FILE__);

Python const PYTHON;

PyThreadState *initPython() {
    debug("Initing Python.");
    Py_InitializeEx(0);
    auto const parent = PyThreadState_Get();
    assert(parent);
    PyEval_InitThreads();
    PyEval_ReleaseThread(parent);
    return parent;
}

}

Python::Python()
        : _parent(initPython()) {
}

Program::Program(Config const& config)
        : _moduleName(config._programModule), _interpreter(PYTHON._parent), _mark(-1) {
    debug("Loading module: %s", _moduleName);
    std::function<void()> const task = [&] {
        PyRef module(PyImport_ImportModule(_moduleName));
        if (module) {
            auto const bytes = module.getAttr("__file__").toPathBytes();
            auto const str = bytes.unwrapBytes();
            debug("Module path: %s", str);
            _path = str;
        }
        else {
            debug("Failed to load module, refresh disabled.");
        }
    };
    _interpreter.runTask(task);
}

void Program::refresh() {
    if (!_path.empty()) {
        auto const mark = boost::filesystem::last_write_time(_path);
        if (mark != _mark) {
            _mark = mark;
            _interpreter = PYTHON._parent;
            debug("Reloading module: %s", _moduleName);
            std::function<void()> const task = [&] {
                _module = PyImport_ImportModule(_moduleName);
                if (_module) {
                    _rate = _module.getAttr("rate").numberToFloatOr(DEFAULT_RATE);
                    debug("Program rate: %.3f", _rate);
                }
                else {
                    debug("Failed to reload module.");
                }
            };
            _interpreter.runTask(task);
        }
    }
}

Program::~Program() {
}

Python::~Python() {
    PyEval_AcquireThread(_parent); // Otherwise Py_Finalize crashes.
    debug("Closing Python.");
    Py_Finalize();
}

void Program::fire(int noteFrame, int offFrameOrNeg, State& state) const {
    if (!noteFrame) {
        state.setLevel4(13); // Use half the available amp.
    }
    if (_module) {
        std::function<void()> const task = [&] {
            if (offFrameOrNeg < 0) {
                _module.getAttr("on").callVoid("(i)", noteFrame);
            }
            else {
                _module.getAttr("off").callVoid("ii", noteFrame, offFrameOrNeg);
            }
            auto const chan = _module.getAttr("chan");
            if (chan) {
                auto const level = chan.getAttr("level");
                if (level) {
                    state.setLevel4(level.numberRoundToInt());
                }
            }
        };
        _interpreter.runTask(task);
    }
}
