#include "mixer.h"

#include "../util/buf.h"

void Mixer::renderImpl() {
    auto const tone = _tone.render(cursor() + _buf.limit());
    if (_state._toneEnabled) {
        _buf.fill(tone.begin());
    }
    else {
        _buf.fill(1); // Fixed and variable levels should work, see qanlgmix and qenvpbuf.
    }
}
