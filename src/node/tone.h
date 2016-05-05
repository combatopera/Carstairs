#pragma once

#include "../config.h"
#include "../node.h"
#include "../state.h"

class Tone: public Node<int> {

    Config *_config;

    int _indexInShape, _progress, _stepSize;

    void renderImpl();

public:

    Tone(Config *config, State *state);

};
