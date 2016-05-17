#include "pcm.h"

#include <stddef.h>

#include "../config.h"
#include "../node.h"
#include "../state.h"
#include "../util/buf.h"
#include "../util/util.h"

PCM::PCM(Config const *config, State *state, Node<float> *naive)
        : Node("PCM", state), _minBLEPs(config), _naive(naive) {
    // Nothing else.
}

void PCM::resetImpl() {
    // Nothing to be done.
}

void PCM::renderImpl() {
    size_t pcmCount = _buf.limit();
    cursor_t naiveX = _naive->cursor(), naiveN = _minBLEPs.getMinNaiveN(naiveX, pcmCount);
    View<float> v = _naive->render(naiveX + naiveN);
    _minBLEPs.paste(naiveX, v, _buf);
}
