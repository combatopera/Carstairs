#include "pcm.h"

#include "../state.h"

PCM::PCM(State *state)
        : Node(state), _tone(state) {
    // Nothing else.
}

void PCM::renderImpl(unsigned long n) {
    _tone.render(_cursor + n); // FIXME: Convert to chip time.
}
