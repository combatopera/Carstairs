#pragma once

#include <ladspa.h>

class Port {

public:

    Port(const char *name, LADSPA_PortDescriptor descriptor, LADSPA_PortRangeHintDescriptor HintDescriptor, LADSPA_Data LowerBound, LADSPA_Data UpperBound);

    ~Port();

    const char *_name;

    LADSPA_PortDescriptor _descriptor;

    LADSPA_PortRangeHint _rangeHint;

};
