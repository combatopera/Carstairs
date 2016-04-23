#pragma once

#include <ladspa.h>

class Port {

public:

    Port(LADSPA_PortDescriptor descriptor, const char *name, LADSPA_PortRangeHintDescriptor HintDescriptor, LADSPA_Data LowerBound, LADSPA_Data UpperBound);

    ~Port();

    LADSPA_PortDescriptor _descriptor;

    const char *_name;

    LADSPA_PortRangeHint _rangeHint;

};
