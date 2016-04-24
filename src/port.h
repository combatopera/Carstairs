#pragma once

#include <ladspa.h>

class PortInfo {

public:

    PortInfo(int ordinal, LADSPA_PortDescriptor descriptor, const char *name, LADSPA_PortRangeHintDescriptor HintDescriptor, LADSPA_Data LowerBound,
            LADSPA_Data UpperBound);

    int const _ordinal;

    LADSPA_PortDescriptor const _descriptor;

    char const * const _name;

    LADSPA_PortRangeHint const _rangeHint;

};
