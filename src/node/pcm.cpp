#include "pcm.h"

#include "../util.h"

void PCM::renderImpl(unsigned long n) {
    zero(_buffer, n);
}
