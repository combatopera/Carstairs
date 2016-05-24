#include "level.h"

#include <cmath>

#include "../util/buf.h"

void Level::resetImpl() {
}

void Level::renderImpl() {
    auto const amp = powf(2, (float(_state.level5()) - 31) / 4) * _maxAmp;
    auto const binary = _tone.render(cursor() + _buf.limit());
    for (auto i = _buf.limit() - 1; SIZEX_NEG != i; --i) {
        _buf.put(i, float(binary.at(i) * 2 - 1) * amp);
    }
}
