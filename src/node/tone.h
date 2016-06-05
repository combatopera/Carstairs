#pragma once

#include "../config.h"
#include "../node.h"
#include "../state.h"
#include "../util/buf.h"

class Tone: public Node<int> {

    int const _atomSize;

    View<int> const _shape;

    int _indexInShape, _progress, _stepSize;

    void startImpl();

    void renderImpl();

public:

    Tone(Config const&, State const&);

};
