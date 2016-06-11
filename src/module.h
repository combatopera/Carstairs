#pragma once

#include <boost/filesystem/path.hpp>

#include "config.h"

class Module {

    boost::filesystem::path const _dir;

public:

    Module(Config const&);

    ~Module();

    boost::filesystem::path const& dir() const {
        return _dir;
    }

};
