#pragma once

#include <ladspa.h>

class PortInfo {

public:

    PortInfo(PortInfo **PORT_INFOS, unsigned *PortCount, LADSPA_PortDescriptor descriptor, const char *name, LADSPA_PortRangeHintDescriptor HintDescriptor,
            LADSPA_Data LowerBound, LADSPA_Data UpperBound);

    LADSPA_PortDescriptor _descriptor;

    const char *_name;

    LADSPA_PortRangeHint _rangeHint;

};
