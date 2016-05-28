#pragma once

#include "state.h"

class Program {

    float const _rate;

    State& _state;

public:

    Program(float rate, State& state)
            : _rate(rate), _state(state) {
    }

    float rate() const {
        return _rate;
    }

    void fire(int, int) const;

};
