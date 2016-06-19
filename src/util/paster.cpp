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

#include "paster.h"

#include "util.h"

namespace {
Log const LOG(__FILE__);
}

void Paster::logRoundingError(DSSI::cursor naiveCursor, DSSI::cursor pcmRef, DSSI::cursor pcmCursor) const {
    CARSTAIRS_INFO("ratio = %.20f", _ratio);
    CARSTAIRS_INFO("naiveX = %lu", naiveCursor);
    CARSTAIRS_INFO("pcmRef = %lu", pcmRef);
    CARSTAIRS_INFO("pcmX = %lu", pcmCursor);
    CARSTAIRS_ERROR("MinBLEP should have started in previous block!");
}
