#pragma once

#include <boost/filesystem/path.hpp>
#include <python3.4m/Python.h>
#include <functional>

#include "../config.h"
#include "main.h"
#include "py.h"

class Interpreter {

    boost::filesystem::path const& _modulesDir;

    PyThreadState *_state;

    Interpreter(boost::filesystem::path const&, Python const&);

public:

    static PyRef import(char const *module) {
        return PyImport_ImportModule(module);
    }

    Interpreter(Config const&, Python const&);

    Interpreter& operator=(Python const&);

    void runTask(std::function<void()> const&) const;

    virtual ~Interpreter();

};
