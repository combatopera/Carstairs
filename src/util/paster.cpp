#include "paster.h"

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

sizex Paster::pcmCountWithOverflow(DSSI::cursor naiveX, DSSI::cursor pcmRef) const {
    sizex pcmRelX, minBLEPIndex;
    pastePrepare(naiveX, pcmRef, pcmRelX, minBLEPIndex);
    return pcmRelX + minBLEPSize(minBLEPIndex);
}

void Paster::pasteMulti(View<float> derivative, DSSI::cursor naiveRef, DSSI::cursor pcmRef, View<float> pcmBuf, sizex dcCursor, float dc) {
    pcmBuf.zero(dcCursor, pcmBuf.limit());
    auto const ampCount = derivative.limit();
    auto ampPtr = derivative.begin();
    auto const lim = _minBLEPs.limit(), step = _minBLEPCount;
    auto const srcPtr = _minBLEPs.begin();
    auto const pcmEnd = pcmBuf.end();
    auto const pcmBegin = const_cast<float *>(pcmBuf.begin());
    auto dcPtr = pcmBegin + dcCursor;
    for (sizex i = 0; i < ampCount; ++i) {
        auto const amp = *ampPtr++;
        if (amp) {
            sizex pcmRelX, minBLEPIndex;
            pastePrepare(naiveRef + i, pcmRef, pcmRelX, minBLEPIndex);
            auto pcmPtr = pcmBegin + pcmRelX;
            assert(pcmPtr + minBLEPSize(minBLEPIndex) <= pcmEnd); // Bounds check. TODO: Do this outside loop.
            // The target must be big enough for a minBLEP at maximum pcmX:
            for (auto k = minBLEPIndex; k < lim; k += step) {
                *pcmPtr++ += amp * srcPtr[k];
            }
            while (dcPtr < pcmPtr) {
                *dcPtr++ += dc;
            }
            dc += amp;
        }
    }
    while (dcPtr < pcmEnd) {
        *dcPtr++ += dc;
    }
}
