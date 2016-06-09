#include "interpreter.h"

#include <boost/filesystem.hpp>
#include <python3.4m/Python.h>
#include <cassert>
#include <fstream>

#include "../util/util.h"

namespace {

Log const LOG(__FILE__);

class Module {

public:

    boost::filesystem::path const _dir;

    Module()
            : _dir(boost::filesystem::temp_directory_path() / boost::filesystem::unique_path()) {
        boost::filesystem::create_directory(_dir);
        std::ofstream f;
        f.open((_dir / "carstairs.py").string().c_str());
        f << R"EOF(class chip: pass
class A: pass
class note: pass
)EOF";
        f.close();
    }

    ~Module() {
        CARSTAIRS_DEBUG("Deleting: %s", _dir.c_str());
        boost::filesystem::remove_all(_dir);
    }

} MODULE;

}

Interpreter::Interpreter(Config const& config, Python const& python)
        : Interpreter(config._modulesDir, python) {
}

Interpreter::Interpreter(boost::filesystem::path const& modulesDir, Python const& python)
        : _modulesDir(modulesDir) {
    CARSTAIRS_DEBUG("Creating new sub-interpreter.");
    PyThreadState * const main = python;
    PyEval_AcquireThread(main);
    _state = Py_NewInterpreter();
    assert(_state);
    assert(main != _state);
    assert(PyThreadState_Get() == _state);
    {
        std::string script;
        script += "import sys\n";
        // Assume neither path contains triple single quotes:
        script += "sys.path.append('''";
        script += MODULE._dir.string();
        script += "''')\n";
        script += "sys.path.append('''";
        script += modulesDir.string();
        script += "''')\n";
        execute(script);
    }
    PyEval_ReleaseThread(_state);
    CARSTAIRS_DEBUG("Created: %p", _state);
}

Interpreter& Interpreter::operator=(Python const& python) {
    CARSTAIRS_REFRESH(Interpreter, _modulesDir, python);
}

void Interpreter::runTask(std::function<void()> const& task) const {
    PyEval_AcquireThread(_state);
    task();
    PyEval_ReleaseThread(_state);
}

Interpreter::~Interpreter() {
    CARSTAIRS_DEBUG("Ending sub-interpreter: %p", _state);
    PyEval_AcquireThread(_state);
    CARSTAIRS_TRACE("Calling: Py_EndInterpreter(%p)", _state);
    Py_EndInterpreter(_state);
    CARSTAIRS_TRACE("Calling: PyEval_ReleaseLock");
    PyEval_ReleaseLock();
    CARSTAIRS_TRACE("Sub-interpreter ended.");
}
