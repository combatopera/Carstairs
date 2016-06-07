#pragma once

#include "../config.h"
#include "../node.h"
#include "../state.h"
#include "../util/buf.h"

class Tone: public Node<int> {

    int const _atomSize;

    View<int> const _shape;

    int _indexInShape, _progress, _stepSize;

    void startImpl() {
        _indexInShape = 0;
        _progress = 0;
    }

    void renderImpl() {
        _stepSize = _atomSize * _state.TP();
        auto const shapeSize = _shape.limit();
        if (_progress >= _stepSize) { // Start a new step.
            _indexInShape = (_indexInShape + 1) % shapeSize;
            _progress = 0;
        }
        sizex endOfStep = _stepSize - _progress, i = 0;
        auto const n = _buf.limit();
        while (endOfStep <= n) { // Could allow next block to extend step by using < here.
            _buf.fill(i, endOfStep, _shape.at(_indexInShape));
            i = endOfStep;
            _indexInShape = (_indexInShape + 1) % shapeSize;
            endOfStep += _stepSize;
        }
        _buf.fill(i, n, _shape.at(_indexInShape));
        _progress = _stepSize - (endOfStep - n);
    }

public:

    Tone(Config const&, State const&);

};
