#include "tone.h"

#include "../buf.h"
#include "../config.h"
#include "../state.h"
#include "../util/util.h"

int shape[] = {1, 0};
int shapeSize = 2;

Tone::Tone(Config *config, State *state)
        : Node("Tone", state), _config(config), _indexInShape(0), _progress(0), _stepSize(0) {
    // Nothing else.
}

void Tone::renderImpl() {
    _stepSize = _config->_atomSize * *_state->TP();
    if (_progress >= _stepSize) { // Start a new step.
        _indexInShape = (_indexInShape + 1) % shapeSize;
        _progress = 0;
    }
    index_t endOfStep = _stepSize - _progress;
    for (index_t i = 0, n = _buf.limit(); i < n;) {
        int value = shape[_indexInShape];
        // Could eagerly terminate step by using > here:
        if (endOfStep >= n) {
            _buf.fill(i, n, value);
            i = n;
            _progress = int(_stepSize - (endOfStep - n));
        }
        else {
            _buf.fill(i, endOfStep, value);
            i = endOfStep;
            _indexInShape = (_indexInShape + 1) % shapeSize;
            _progress = 0;
            endOfStep += _stepSize;
        }
    }
}
