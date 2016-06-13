#pragma once

#include "../node.h"
#include "../state.h"
#include "../util/buf.h"

class Shape {

protected:

    Buffer<int> _data;

public:

    Shape(char const *label, sizex limit)
            : _data(label, limit) {
    }

    virtual ~Shape() {
    }

    virtual void step(sizex& index) const = 0;

    Buffer<int> const& data() const {
        return _data;
    }

};

class PeriodicShape: public Shape {

public:

    PeriodicShape(char const *label, sizex limit)
            : Shape(label, limit) {
    }

    void step(sizex& index) const {
        index = (index + 1) % _data.limit(); // TODO: Optimise out this division.
    }

};

class Osc: public Node<int> {

    sizex const _atomSize;

    Shape const *_shape;

    int const& _period;

    bool const _eagerStepSize;

#ifdef CARSTAIRS_TEST
public:
#endif

    sizex _indexInShape;

    int _progress, _stepSize;

private:

    void startImpl() {
        _indexInShape = 0;
        _progress = 0;
    }

    inline void updateStepSize() {
        _stepSize = _atomSize * _period;
    }

    void renderImpl() {
        auto const& shapeData = _shape->data();
        auto const n = _buf.limit();
        if (_eagerStepSize || !_progress) {
            updateStepSize();
        }
        if (_progress >= _stepSize) { // Start a new step.
            _shape->step(_indexInShape);
            _progress = 0;
        }
        sizex endOfStep = _stepSize - _progress, i = 0;
        auto ptr = const_cast<int *>(_buf.begin());
        if (endOfStep <= n) {
            if (!_eagerStepSize) {
                updateStepSize();
            }
            do {
                auto const val = shapeData.at(_indexInShape);
                for (; i < endOfStep; ++i) {
                    *ptr++ = val;
                }
                _shape->step(_indexInShape);
                endOfStep += _stepSize;
            } while (endOfStep <= n);
        }
        auto const val = shapeData.at(_indexInShape);
        for (; i < n; ++i) {
            *ptr++ = val;
        }
        _progress = _stepSize - (endOfStep - n);
    }

protected:

    void setShape(Shape const& shape) {
        _shape = &shape;
        startImpl();
    }

    Osc(sizex atomSize, State const& state, char const *label, Shape const& shape, int const& period, bool eagerStepSize)
            : Node(label, state), _atomSize(atomSize), _shape(&shape), _period(period), _eagerStepSize(eagerStepSize), //
            _indexInShape(), _progress(), _stepSize() {
    }

};
