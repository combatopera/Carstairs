#include "tone.h"

static class Square: public Buffer<int> {

public:

    Square()
            : Buffer("SQUARE", 2) {
        put(0, 1);
        put(1, 0);
    }

} SQUARE;

Tone::Tone(Config const& config, State& state)
        : Node("Tone", state), _config(config), _shape(SQUARE), _indexInShape(INITIAL_INDEX_IN_SHAPE), _progress(INITIAL_PROGRESS), _stepSize(0) {
}

void Tone::resetImpl() {
    _indexInShape = INITIAL_INDEX_IN_SHAPE;
    _progress = INITIAL_PROGRESS;
}

void Tone::renderImpl() {
    _stepSize = _config._atomSize * _state.TP();
    if (_progress >= _stepSize) { // Start a new step.
        _indexInShape = (_indexInShape + 1) % (int) _shape.limit();
        _progress = 0;
    }
    sizex endOfStep = _stepSize - _progress;
    auto const n = _buf.limit();
    for (sizex i = 0; i < n;) {
        auto value = _shape.at(_indexInShape);
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
