#include <dssi.h>
#include <ladspa.h>

class Descriptor {

    LADSPA_PortDescriptor *_PortDescriptors;

    LADSPA_PortRangeHint *_PortRangeHints;

    const char **_PortNames;

    LADSPA_Descriptor _ladspaDescriptor;

public:

    DSSI_Descriptor _dssiDescriptor;

    Descriptor();

    ~Descriptor();

};
