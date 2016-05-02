#include "port.h"

#include <ladspa.h>

#include "../util/util.h"

PortInfo_t::PortInfo_t(index_t ordinal, //
        bool output, //
        bool audio, //
        const char *name, //
        LADSPA_PortRangeHintDescriptor HintDescriptor, //
        LADSPA_Data LowerBound, //
        LADSPA_Data UpperBound, //
        int controllers)
        : _ordinal(ordinal), //
        _descriptor((output ? LADSPA_PORT_OUTPUT : LADSPA_PORT_INPUT) | (audio ? LADSPA_PORT_AUDIO : LADSPA_PORT_CONTROL)), //
        _name(name), //
        _rangeHint {HintDescriptor, LowerBound, UpperBound}, //
        _controllers(controllers) {
    // Nothing else.
}
