#pragma once

#include "../node.h"
#include "../state.h"

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
