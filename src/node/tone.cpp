#include "tone.h"

#include <cmath>

#include "../buf.h"
#include "../state.h"
#include "../util/util.h"

int shape[] = {1, 0};
int shapeSize = 2;

Tone::Tone(State *state)
        : Node("Tone", state), _indexInShape(0), _progress(0), _stepSize(0) {
    // Nothing else.
}

int const REF_MIDI_NOTE = 69, REF_FREQ = 440, SEMITONES = 12, CLOCK = 2000000, ATOM_SIZE = 8;

void Tone::renderImpl() {
    double freq = REF_FREQ * pow(2, ((_state->_midiNote - REF_MIDI_NOTE) / (double) SEMITONES));
    int TP = (int) round(CLOCK / (16 * freq));
    _stepSize = ATOM_SIZE * TP;
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
