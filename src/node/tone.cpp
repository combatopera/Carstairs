#include "tone.h"

#include "../config.h"
#include "../state.h"
#include "../util/buf.h"
#include "../util/util.h"

static class Square: public Buffer<int> {

public:

    Square()
            : Buffer("SQUARE", 2) {
        put(0, 1);
        put(1, 0);
    }

} SQUARE;

Tone::Tone(Config const *config, State *state)
        : Node("Tone", state), _config(config), _shape(SQUARE), _indexInShape(INITIAL_INDEX_IN_SHAPE), _progress(INITIAL_PROGRESS), _stepSize(0) {
}

void Tone::resetImpl() {
    _indexInShape = INITIAL_INDEX_IN_SHAPE;
    _progress = INITIAL_PROGRESS;
}

void Tone::renderImpl() {
    _stepSize = _config->_atomSize * _state->TP();
    if (_progress >= _stepSize) { // Start a new step.
        _indexInShape = (_indexInShape + 1) % (int) _shape.limit();
        _progress = 0;
    }
    index_t endOfStep = _stepSize - _progress;
    for (index_t i = 0, n = _buf.limit(); i < n;) {
        int value = _shape.at(_indexInShape);
        // Could allow next block to extend step by using >= here:
        if (endOfStep > n) {
            _buf.fill(i, n, value);
            i = n;
            _progress = int(_stepSize - (endOfStep - n));
        }
        else {
            _buf.fill(i, endOfStep, value);
            i = endOfStep;
            _indexInShape = (_indexInShape + 1) % (int) _shape.limit();
            _progress = 0;
            endOfStep += _stepSize;
        }
    }
}
