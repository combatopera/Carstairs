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
