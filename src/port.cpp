#include "port.h"

#include <ladspa.h>

Port::Port(const char *name, LADSPA_PortDescriptor descriptor, LADSPA_PortRangeHintDescriptor HintDescriptor, LADSPA_Data LowerBound, LADSPA_Data UpperBound)
        : _name(name), _descriptor(descriptor) {
    _rangeHint = {HintDescriptor, LowerBound, UpperBound};
}

Port::~Port() {
    // Nothing to be done.
}
