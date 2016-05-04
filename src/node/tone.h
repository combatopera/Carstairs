#pragma once

#include "../node.h"
#include "../state.h"

class Tone: public Node<int> {

    int _indexInShape, _progress, _stepSize;

    void renderImpl();

public:

    Tone(State *state);

};
