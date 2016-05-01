#pragma once

#include "../node.h"
#include "../state.h"

class Tone: public Node<int> {

    void renderImpl(unsigned long n);

public:

    Tone(State *state);

};
