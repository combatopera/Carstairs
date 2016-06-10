#pragma once

#include <boost/filesystem/path.hpp>

class Module {

    boost::filesystem::path const _dir;

public:

    Module();

    ~Module();

    boost::filesystem::path const& dir() const {
        return _dir;
    }

};
