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

#include "module.h"

#include <boost/filesystem/operations.hpp>
#include <boost/format.hpp>
#include <fstream>
#include <string>

Log const Module::LOG(__FILE__);

namespace {

char const * const CODE =
#include "module.raw"
;

}

Module::Module(Config const& config)
        : _dir(boost::filesystem::temp_directory_path() / boost::filesystem::unique_path()) {
    boost::filesystem::create_directory(_dir);
    std::ofstream f;
    f.open((_dir / "carstairs.py").string().c_str());
    f << boost::format(CODE) % config.YM2149_ATOM_SIZE % config._nominalClock;
    f.close();
}

Module::~Module() {
    CARSTAIRS_DEBUG("Deleting: %s", _dir.c_str());
    boost::filesystem::remove_all(_dir);
}
