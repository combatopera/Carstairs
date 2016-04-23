#include <dssi.h>
#include <ladspa.h>

class Descriptor {

    LADSPA_PortDescriptor *_PortDescriptors;

    const char **_PortNames;

    LADSPA_PortRangeHint *_PortRangeHints;

    LADSPA_Descriptor _ladspaDescriptor;

public:

    DSSI_Descriptor _dssiDescriptor;

    Descriptor();

    ~Descriptor();

};
