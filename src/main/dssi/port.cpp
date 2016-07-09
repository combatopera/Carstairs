// Copyright 2016 Andrzej Cichocki

// This file is part of Carstairs.
//
// Carstairs is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Carstairs is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Carstairs.  If not, see <http://www.gnu.org/licenses/>.

#include "port.h"

PortInfo_t::PortInfo_t(sizex ordinal, //
        bool output, //
        bool audio, //
        char const *name, //
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
