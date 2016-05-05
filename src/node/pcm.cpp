#include "pcm.h"

#include <stddef.h>

#include "../buf.h"
#include "../config.h"
#include "../node.h"
#include "../state.h"
#include "../util/util.h"

PCM::PCM(Config *config, State *state)
        : Node("PCM", state), _tone(config, state) {
    // Nothing else.
}

void PCM::renderImpl() {
    size_t pcmCount = _buf.limit();
    cursor_t naiveX = _tone.cursor(), naiveN = _minBLEPs.getMinNaiveN(naiveX, pcmCount);
    _minBLEPs.paste(naiveX, _tone.render(naiveX + naiveN), _buf);
}
