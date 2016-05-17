#include "pcm.h"

#include <stddef.h>

#include "../config.h"
#include "../node.h"
#include "../state.h"
#include "../util/buf.h"
#include "../util/util.h"

PCM::PCM(Config const *config, State *state, Node<float> *naive)
        : Node("PCM", state), _minBLEPs(config), _naive(naive), _dc(INITIAL_DC) {
    // Nothing else.
}

void PCM::resetImpl() {
    _dc = INITIAL_DC;
}

void PCM::renderImpl() {
    size_t pcmCount = _buf.limit();
    cursor_t naiveX = _naive->cursor(), naiveN = _minBLEPs.getMinNaiveN(naiveX, pcmCount);
    View<float> naive = _naive->render(naiveX + naiveN);
    _derivative.setLimit(naive.limit());
    _derivative.fill(naive.begin());
    _derivative.differentiate(_dc);
    if (naive.limit()) { // Otherwise _dc doesn't change.
        _dc = naive.at(naive.limit() - 1);
    }
    _minBLEPs.paste(naiveX, naive, _buf);
}
