#include "tone.h"

#include "../state.h"
#include "../util.h"

Tone::Tone(State *state)
        : Node(state) {
    // Nothing else.
}

void Tone::renderImpl(unsigned long n) {
    zero(_buffer, n);
}
