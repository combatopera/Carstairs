#pragma once

#include <ladspa.h>

class Port {

public:

    Port(const char *name, LADSPA_PortDescriptor descriptor);

    const char *_name;

    LADSPA_PortDescriptor _descriptor;

};
