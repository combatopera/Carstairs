#include "port.h"

#include <ladspa.h>

Port::Port(const char *name, LADSPA_PortDescriptor descriptor, LADSPA_PortRangeHintDescriptor HintDescriptor, LADSPA_Data LowerBound, LADSPA_Data UpperBound)
        : _name(name), _descriptor(descriptor) {
    _rangeHintPtr = new LADSPA_PortRangeHint{HintDescriptor, LowerBound, UpperBound};
}

Port::~Port() {
    delete _rangeHintPtr;
}
