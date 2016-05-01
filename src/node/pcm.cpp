#include "pcm.h"

#include "../state.h"
#include "../util.h"

PCM::PCM(State *state)
        : Node(state), _tone(state) {
    // Nothing else.
}

void PCM::renderImpl(unsigned long n) {
    zero(_buffer, n);
}
