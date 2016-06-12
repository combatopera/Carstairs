#pragma once

#include "../node.h"
#include "../state.h"
#include "../util/buf.h"

class Osc: public Node<int> {

    sizex const _atomSize;

    View<int> _shape;

    int const& _period;

    bool const _eagerStepSize;

#ifdef CARSTAIRS_TEST
public:
#endif

    int _indexInShape, _progress, _stepSize;

private:

    void startImpl() {
        _indexInShape = 0;
        _progress = 0;
    }

    inline void updateStepSize() {
        _stepSize = _atomSize * _period;
    }

    inline void nextVal(sizex const shapeSize) {
        _indexInShape = (_indexInShape + 1) % shapeSize; // TODO: Optimise out this division.
    }

    void renderImpl() {
        auto const shapeSize = _shape.limit(), n = _buf.limit();
        if (_eagerStepSize || !_progress) {
            updateStepSize();
        }
        if (_progress >= _stepSize) { // Start a new step.
            nextVal(shapeSize);
            _progress = 0;
        }
        sizex endOfStep = _stepSize - _progress, i = 0;
        auto ptr = const_cast<int *>(_buf.begin());
        if (endOfStep <= n) {
            if (!_eagerStepSize) {
                updateStepSize();
            }
            do {
                auto const val = _shape.at(_indexInShape);
                for (; i < endOfStep; ++i) {
                    *ptr++ = val;
                }
                nextVal(shapeSize);
                endOfStep += _stepSize;
            } while (endOfStep <= n);
        }
        auto const val = _shape.at(_indexInShape);
        for (; i < n; ++i) {
            *ptr++ = val;
        }
        _progress = _stepSize - (endOfStep - n);
    }

protected:

    void setShape(View<int> shape) {
        _shape = shape;
        startImpl();
    }

    Osc(sizex atomSize, State const& state, char const *label, View<int> const shape, int const& period, bool eagerStepSize)
            : Node(label, state), _atomSize(atomSize), _shape(shape), _period(period), _eagerStepSize(eagerStepSize), //
            _indexInShape(), _progress(), _stepSize() {
    }

};
