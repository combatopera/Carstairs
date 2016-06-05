#pragma once

#include "../config.h"
#include "../node.h"
#include "../state.h"
#include "../util/buf.h"

#ifdef CARSTAIRS_TEST
Buffer<int> const& noiseShape();
#endif

class Noise: public Node<int> {

    int const _atomSize;

    View<int> const _shape;

    int _indexInShape, _progress, _stepSize;

    void startImpl() {
    }

    void renderImpl() {
    }

public:

    Noise(Config const& config, State const& state);

};
