#pragma once

#include <ladspa.h>

#include "../util/buf.h"

class PortInfo_t {

    sizex const _ordinal;

public:

    PortInfo_t(sizex ordinal, bool output, bool audio, char const *name, LADSPA_PortRangeHintDescriptor HintDescriptor, LADSPA_Data LowerBound,
            LADSPA_Data UpperBound, int controllers);

    LADSPA_PortDescriptor const _descriptor;

    char const * const _name;

    LADSPA_PortRangeHint const _rangeHint;

    int const _controllers;

    sizex ordinal() const {
        return _ordinal;
    }

};
