#include "pcm.h"

#include <stddef.h>

#include "../config.h"
#include "../node.h"
#include "../state.h"
#include "../util/buf.h"
#include "../util/util.h"
#include "level.h"

PCM::PCM(Config const *config, State *state, Level *level)
        : Node("PCM", state), _minBLEPs(config), _level(level) {
    // Nothing else.
}

void PCM::resetImpl() {
    // Nothing to be done.
}

void PCM::renderImpl() {
    size_t pcmCount = _buf.limit();
    cursor_t naiveX = _level->cursor(), naiveN = _minBLEPs.getMinNaiveN(naiveX, pcmCount);
    View<float> v = _level->render(naiveX + naiveN);
    _minBLEPs.paste(naiveX, v, _buf);
}
