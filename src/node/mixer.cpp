#include "mixer.h"

#include "../util/buf.h"

void Mixer::renderImpl() {
    auto const tone = _tone.render(cursor() + _buf.limit());
    auto const noise = _noise.render(cursor() + _buf.limit());
    if (_state.toneFlag()) {
        _buf.fill(tone.begin());
        if (_state.noiseFlag()) {
            _buf.AND(noise.begin()); // We use AND as zero is preferred over envelope, see qbmixenv.
        }
    }
    else if (_state.noiseFlag()) {
        _buf.fill(noise.begin());
    }
    else {
        _buf.fill(1); // Fixed and variable levels should work, see qanlgmix and qenvpbuf.
    }
}
