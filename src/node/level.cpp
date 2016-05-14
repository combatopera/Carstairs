#include "level.h"

#include <cmath>

#include "../buf.h"
#include "../node.h"
#include "../state.h"
#include "../util/util.h"
#include "tone.h"

void Level::resetImpl() {
}

void Level::renderImpl() {
    float amp = powf(2, (float(_state->level5()) - 31) / 4);
    View<int> v = _tone->render(cursor() + _buf.limit());
    for (index_t i = 0, n = _buf.limit(); i < n; ++i) {
        _buf.put(i, float(v.at(i)) * amp);
    }
}
