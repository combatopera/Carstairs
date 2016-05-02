#include "minbleps.h"

#include <ladspa.h>
#include <cmath>

#include "../buf.h"
#include "util.h"

static int const SCALE = (int) round(2000000 / 44100.);

cursor_t MinBLEPs::getMinNaiveN(cursor_t naiveX, cursor_t pcmCount) {
    return pcmCount * SCALE; // FIXME: Do it properly.
}

void MinBLEPs::paste(cursor_t naiveX, View<int> naiveBuf, View<LADSPA_Data> pcmBuf) {
    for (cursor_t pcmI = 0; pcmI < pcmBuf.limit(); ++pcmI) {
        LADSPA_Data acc = 0;
        for (int s = 0; s < SCALE; ++s) {
            acc += (LADSPA_Data) naiveBuf.at(pcmI * SCALE + s);
        }
        pcmBuf.put(pcmI, acc / SCALE);
    }
}
