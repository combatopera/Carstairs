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
