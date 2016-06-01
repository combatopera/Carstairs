#pragma once

#include <dssi.h>
#include <ladspa.h>
#include <climits>

#include "../config.h"

namespace DSSI {

typedef unsigned long cursor;

cursor const CURSOR_MAX = ULONG_MAX;

}

class Descriptors {

    LADSPA_PortDescriptor * const _PortDescriptors;

    char const ** const _PortNames;

    LADSPA_PortRangeHint * const _PortRangeHints;

    LADSPA_Descriptor _ladspaDescriptor;

    DSSI_Descriptor _dssiDescriptor;

    Descriptors(Descriptors const&); // Make cppcheck happy.

public:

    DSSI_Descriptor const *dssiDescriptor() const;

    Descriptors(Config const&);

    ~Descriptors();

};
