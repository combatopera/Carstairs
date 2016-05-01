#include "port.h"

#include <ladspa.h>

PortInfo::PortInfo(int ordinal, LADSPA_PortDescriptor descriptor, const char *name, LADSPA_PortRangeHintDescriptor HintDescriptor, LADSPA_Data LowerBound,
        LADSPA_Data UpperBound, int controllers)
        : _ordinal(ordinal), _descriptor(descriptor), _name(name), _rangeHint {HintDescriptor, LowerBound, UpperBound}, _controllers(controllers) {
    // Nothing else.
}
