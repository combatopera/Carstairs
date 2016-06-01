#pragma once

#include <dssi.h>
#include <ladspa.h>
#include <climits>
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

class Programs {

    static DSSI::cursor const BANK = 0;

    std::vector<DSSI_Program_Descriptor> _programs;

public:

    Programs(Config const&, Python const&);

    ~Programs();

    DSSI_Program_Descriptor const *programOrNull(sizex index) const {
        return index < _programs.size() ? &_programs[index] : 0;
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
