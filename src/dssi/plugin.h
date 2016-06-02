#pragma once

#include <dssi.h>
#include <ladspa.h>
#include <climits>
#include <memory>
#include <string>
#include <vector>

#include "../config.h"
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

public:

    ProgramInfo(std::string const& name)
            : _name(name) { // Copy.
        _descriptor.Name = _name.c_str();
    }

    bool operator<(ProgramInfo const& that) const {
        return _name < that._name;
    }

    void setProgram(DSSI::cursor program) {
        _descriptor.Program = program;
    }

    DSSI_Program_Descriptor const& descriptor() const {
        return _descriptor;
    }

};

class Programs {

    std::vector<std::unique_ptr<ProgramInfo>> _programs;

public:

    static DSSI::cursor const BANK = 0;

    Programs(Config const&, Python const&);

    DSSI_Program_Descriptor const *programOrNull(sizex index) const {
        return index < _programs.size() ? &_programs[index].get()->descriptor() : 0;
    }

    sizex size() const {
        return sizex(_programs.size());
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

    DSSI_Descriptor const *dssiDescriptor() const;

    Descriptors(Config const&, Values<PortInfo_t const> const&);

    ~Descriptors();

};
