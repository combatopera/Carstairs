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
        : Node("Tone", state), _atomSize(config._atomSize), _shape(SQUARE), _indexInShape(), _progress(), _stepSize() {
}

void Tone::startImpl() {
    _indexInShape = 0;
    _progress = 0;
}

void Tone::renderImpl() {
    _stepSize = _atomSize * _state.TP();
    if (_progress >= _stepSize) { // Start a new step.
        _indexInShape = (_indexInShape + 1) % _shape.limit();
        _progress = 0;
    }
    sizex endOfStep = _stepSize - _progress, i = 0;
    auto const n = _buf.limit();
    while (true) {
        // Could allow next block to extend step by using >= here:
        if (endOfStep > n) {
            break;
        }
        _buf.fill(i, endOfStep, _shape.at(_indexInShape));
        i = endOfStep;
        _indexInShape = (_indexInShape + 1) % _shape.limit();
        _progress = 0;
        endOfStep += _stepSize;
    }
    _buf.fill(i, n, _shape.at(_indexInShape));
    _progress = _stepSize - (endOfStep - n);
}
