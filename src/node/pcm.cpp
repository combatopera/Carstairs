#include "pcm.h"

#include <stddef.h>

#include "../buf.h"
#include "../config.h"
#include "../node.h"
#include "../state.h"
#include "../util/util.h"
#include "tone.h"

PCM::PCM(Config const *config, State *state, Tone *tone)
        : Node("PCM", state), _tone(tone) {
    // Nothing else.
}

void PCM::resetImpl() {
    // Nothing to be done.
}

void PCM::renderImpl() {
    size_t pcmCount = _buf.limit();
    cursor_t naiveX = _tone->cursor(), naiveN = _minBLEPs.getMinNaiveN(naiveX, pcmCount);
    View<int> v = _tone->render(naiveX + naiveN);
    _minBLEPs.paste(naiveX, v, _buf);
}
