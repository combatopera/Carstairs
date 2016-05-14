#include "minbleps.h"

#include <ladspa.h>
#include <cmath>

#include "../buf.h"
#include "../config.h"
#include "util.h"

MinBLEPs::MinBLEPs(Config const *config)
        : _scale((int) roundf(config->pluginClock() / config->_pcmRate)) {
}

cursor_t MinBLEPs::getMinNaiveN(cursor_t naiveX, cursor_t pcmCount) const {
    return pcmCount * _scale; // FIXME: Do it properly.
}

void MinBLEPs::paste(cursor_t naiveX, View<float> naiveBuf, View<LADSPA_Data> pcmBuf) const {
    for (cursor_t pcmI = 0; pcmI < pcmBuf.limit(); ++pcmI) {
        LADSPA_Data acc = 0;
        for (int s = 0; s < _scale; ++s) {
            acc += naiveBuf.at(pcmI * _scale + s);
        }
        pcmBuf.put(pcmI, acc / float(_scale));
    }
}
