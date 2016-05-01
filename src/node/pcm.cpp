#include "pcm.h"

#include "../state.h"
#include "../util.h"

PCM::PCM(State *state)
        : Node(state), _tone(state) {
    // Nothing else.
}

void PCM::renderImpl() {
    _tone.render(_cursor + _buf.limit()); // FIXME: Convert to chip time.
}
