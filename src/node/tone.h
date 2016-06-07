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
        auto ptr = const_cast<int *>(_buf.begin());
        while (endOfStep <= n) { // Could allow next block to extend step by using < here.
            auto const val = _shape.at(_indexInShape);
            for (; i < endOfStep; ++i) {
                *ptr++ = val;
            }
            _indexInShape = (_indexInShape + 1) % shapeSize;
            endOfStep += _stepSize;
        }
        auto const val = _shape.at(_indexInShape);
        for (; i < n; ++i) {
            *ptr++ = val;
        }
        _progress = _stepSize - (endOfStep - n);
    }

public:

    Tone(Config const&, State const&);

};
