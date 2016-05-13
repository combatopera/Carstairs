#pragma once

#include <dssi.h>
#include <ladspa.h>

class Descriptors {

    LADSPA_PortDescriptor *_PortDescriptors;

    char const **_PortNames;

    LADSPA_PortRangeHint *_PortRangeHints;

    LADSPA_Descriptor _ladspaDescriptor;

    DSSI_Descriptor _dssiDescriptor;

public:

    DSSI_Descriptor const *dssiDescriptor() const;

    Descriptors();

    ~Descriptors();

};
