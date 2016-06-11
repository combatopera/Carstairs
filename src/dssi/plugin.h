#pragma once

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <dssi.h>
#include <ladspa.h>
#include <climits>
#include <ctime>
#include <memory>
#include <string>
#include <vector>

#include "../config.h"
#include "../module.h"
#include "../py/main.h"
#include "../util/buf.h"
#include "../util/enum.h"
#include "port.h"

namespace DSSI {

typedef unsigned long cursor;

cursor const CURSOR_MAX = ULONG_MAX;

}

class ProgramInfo {

    std::string const _name;

    DSSI_Program_Descriptor _descriptor;

    boost::filesystem::path _path;

public:

    static DSSI::cursor const BANK = 0;

    ProgramInfo(boost::filesystem::path const& path, std::string const& name, sizex index)
            : _name(name), _path(path) { // Copies.
        _descriptor.Bank = BANK;
        _descriptor.Name = _name.c_str();
    }

    bool operator<(ProgramInfo const& that) const {
        return _name < that._name;
    }

    void setIndex(sizex index) {
        _descriptor.Program = index;
    }

    sizex index() const {
        return sizex(_descriptor.Program);
    }

    DSSI_Program_Descriptor const& descriptor() const {
        return _descriptor;
    }

    std::time_t lastWriteTime() const {
        return boost::filesystem::last_write_time(_path);
    }

};

class ProgramInfos {

    std::vector<std::unique_ptr<ProgramInfo>> _infos;

    void addOrLog(boost::filesystem::path const&, std::string const&);

public:

    ProgramInfos(Config const&, Module const&, Python const&);

    DSSI_Program_Descriptor const *descriptorOrNull(sizex index) const {
        return index < _infos.size() ? &_infos[index].get()->descriptor() : 0;
    }

    sizex size() const {
        return sizex(_infos.size());
    }

    ProgramInfo& operator[](sizex i) const {
        return *_infos[i].get();
    }

};

class Descriptors {

    LADSPA_PortDescriptor * const _PortDescriptors;

    char const ** const _PortNames;

    LADSPA_PortRangeHint * const _PortRangeHints;

    LADSPA_Descriptor _ladspaDescriptor;

    DSSI_Descriptor _dssiDescriptor;

    Descriptors(Descriptors const&); // Make cppcheck happy.

public:

    DSSI_Descriptor const& dssiDescriptor() const {
        return _dssiDescriptor;
    }

    Descriptors(Config const&, Values<PortInfo_t const> const&);

    ~Descriptors();

};
