#include "tone.h"

#include "../util.h"

void Tone::renderImpl(unsigned long n) {
    zero(_buffer, n);
}
