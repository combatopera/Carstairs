#pragma once

#include "../node.h"
#include "../state.h"
#include "../util/buf.h"

#ifdef CARSTAIRS_TEST
extern Buffer<int> NOISE;
#endif

class Noise: public Node<int> {

    void startImpl() {
    }

    void renderImpl() {
    }

public:

    Noise(State const& state)
            : Node("Noise", state) {
    }

};
