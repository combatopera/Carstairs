#pragma once

#include "../node.h"
#include "../state.h"

class Tone: public Node<int> {

    void renderImpl();

public:

    Tone(State *state);

};
