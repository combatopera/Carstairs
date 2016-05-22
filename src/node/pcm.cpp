#include "pcm.h"

#include <stddef.h>

#include "../util/util.h"

PCM::PCM(Config const& config, State *state, Node<float> *naive)
        : Node("PCM", state), _minBLEPs(config), _naive(naive), _dc(INITIAL_DC) {
    // Nothing else.
}

void PCM::resetImpl() {
    _dc = INITIAL_DC;
}

void PCM::renderImpl() {
    size_t pcmCount = _buf.limit();
    DSSI::cursor newNaiveX = _minBLEPs.pcmXToNaiveX(cursor() + pcmCount);
    View<float> naive = _naive->render(newNaiveX);
    _derivative.snapshot(naive);
    _derivative.differentiate(_dc);
    if (naive.limit()) { // Otherwise _dc doesn't change.
        _dc = naive.at(naive.limit() - 1);
    }
    for (DSSI::cursor pcmI = 0; pcmI < pcmCount; ++pcmI) {
        _buf.put(pcmI, naive.at(pcmI * 2000000 / 44100));
    }
}
