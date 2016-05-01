#include "tone.h"

#include "../state.h"

Tone::Tone(State *state)
        : Node(state) {
    // Nothing else.
}

void Tone::renderImpl() {
    _buf.zero();
}
