#pragma once

#include "../config.h"
#include "../node.h"
#include "../state.h"
#include "../util/buf.h"

class Tone: public Node<int> {

    static int const INITIAL_INDEX_IN_SHAPE = 0;

    static int const INITIAL_PROGRESS = 0;

    Config const * const _config;

    View<int> _shape;

    int _indexInShape, _progress, _stepSize;

    void resetImpl();

    void renderImpl();

public:

    Tone(Config const *config, State *state);

};
