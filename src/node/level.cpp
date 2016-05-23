#include "level.h"

#include <cmath>

#include "../util/buf.h"

void Level::resetImpl() {
}

void Level::renderImpl() {
    auto amp = powf(2, (float(_state.level5()) - 31) / 4) * _maxAmp;
    auto binary = _tone.render(cursor() + _buf.limit());
    for (unsigned i = 0, n = unsigned(_buf.limit()); i < n; ++i) {
        _buf.put(i, float(binary.at(i) * 2 - 1) * amp);
    }
}
