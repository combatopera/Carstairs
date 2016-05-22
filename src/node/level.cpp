#include "level.h"

#include <cmath>

#include "../util/buf.h"
#include "../util/util.h"

void Level::resetImpl() {
}

void Level::renderImpl() {
    float amp = powf(2, (float(_state->level5()) - 31) / 4) * _maxAmp;
    View<int> v = _tone.render(cursor() + _buf.limit());
    for (index_t i = 0, n = _buf.limit(); i < n; ++i) {
        _buf.put(i, float(v.at(i) * 2 - 1) * amp);
    }
}
