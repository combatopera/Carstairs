#pragma once

#include <ladspa.h>

class PortInfo_t {

public:

    PortInfo_t(int ordinal, bool output, bool audio, const char *name, LADSPA_PortRangeHintDescriptor HintDescriptor, LADSPA_Data LowerBound,
            LADSPA_Data UpperBound, int controllers);

    int const _ordinal;

    LADSPA_PortDescriptor const _descriptor;

    char const * const _name;

    LADSPA_PortRangeHint const _rangeHint;

    int const _controllers;

};
