#include "port.h"

#include <ladspa.h>

PortInfo::PortInfo(PortInfo **PORT_INFOS, unsigned *PortCount, LADSPA_PortDescriptor descriptor, const char *name,
        LADSPA_PortRangeHintDescriptor HintDescriptor, LADSPA_Data LowerBound, LADSPA_Data UpperBound)
        : _descriptor(descriptor), _name(name) {
    _rangeHint = {HintDescriptor, LowerBound, UpperBound};
    unsigned i = *PortCount;
    PORT_INFOS[i] = this;
    *PortCount = i + 1;
}
