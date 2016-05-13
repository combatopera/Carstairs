#pragma once

#include <dssi.h>
#include <ladspa.h>

class Descriptors {

    LADSPA_PortDescriptor _PortDescriptors[];

    char const *_PortNames[];

    LADSPA_PortRangeHint _PortRangeHints[];

    LADSPA_Descriptor _ladspaDescriptor;

public:

    DSSI_Descriptor _dssiDescriptor;

    Descriptors();

    ~Descriptors();

};
