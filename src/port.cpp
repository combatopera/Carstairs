#include "port.h"

#include <ladspa.h>

Port::Port(const char *name, LADSPA_PortDescriptor descriptor, LADSPA_PortRangeHintDescriptor HintDescriptor, LADSPA_Data LowerBound, LADSPA_Data UpperBound)
        : _name(name), _descriptor(descriptor) {
    _rangeHintPtr = new LADSPA_PortRangeHint;
    _rangeHintPtr->HintDescriptor = HintDescriptor;
    _rangeHintPtr->LowerBound = LowerBound;
    _rangeHintPtr->UpperBound = UpperBound;
}

Port::~Port() {
    delete _rangeHintPtr;
}
