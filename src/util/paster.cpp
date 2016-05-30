#include "paster.h"

#include <cassert>
#include <cmath>

#include "util.h"

namespace {
Log const LOG(__FILE__);
}

inline void pastePrepare(Paster const& paster, DSSI::cursor naiveX, DSSI::cursor pcmRef, sizex& pcmRelX, sizex& minBLEPIndex) {
    auto const pcmMark = double(naiveX) * paster._ratio;
    // If pcmX is 1 too big due to rounding error, we simply skip _minBLEPs[0] which is close to zero:
    auto const pcmX = DSSI::cursor(ceil(pcmMark));
    if (pcmX < pcmRef) {
        info("ratio = %.20f", paster._ratio);
        info("naiveX = %lu", naiveX);
        info("pcmRef = %lu", pcmRef);
        info("pcmX = %lu", pcmX);
        error("MinBLEP should have started in previous block!");
        assert(false);
    }
    pcmRelX = sizex(pcmX - pcmRef);
    auto const distance = double(pcmX) - pcmMark;
    minBLEPIndex = sizex(round(distance * paster._minBLEPCount));
}

sizex Paster::pcmCountWithOverflow(DSSI::cursor naiveX, DSSI::cursor pcmRef) const {
    sizex pcmRelX, minBLEPIndex;
    pastePrepare(*this, naiveX, pcmRef, pcmRelX, minBLEPIndex);
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
            pastePrepare(*this, naiveRef + i, pcmRef, pcmRelX, minBLEPIndex);
            auto pcmPtr = pcmBegin + pcmRelX;
            assert(pcmPtr + minBLEPSize(minBLEPIndex) <= pcmEnd); // Bounds check.
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
