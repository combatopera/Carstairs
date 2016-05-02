#include "tone.h"

#include "../state.h"

Tone::Tone(State *state)
        : Node("Tone", state) {
    // Nothing else.
}

void Tone::renderImpl() {
    _buf.zero();
}
