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

#include <boost/filesystem/path.hpp>
#include <python3.4m/Python.h>
#include <functional>
#include <string>

#include "../config.h"
#include "../module.h"
#include "main.h"
#include "py.h"

class Interpreter {

    PyThreadState *_state;

    Interpreter(boost::filesystem::path const&, Module const&, Python const&);

public:

    static PyRef import(char const *module) {
        return PyImport_ImportModule(module);
    }

    static void execute(std::string const& script) {
        PyRun_SimpleString(script.c_str());
    }

    Interpreter(Config const&, Module const&, Python const&);

    void runTask(std::function<void()> const&) const;

    virtual ~Interpreter();

};
