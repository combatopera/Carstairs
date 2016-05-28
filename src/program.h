#pragma once

#include "dssi/plugin.h"
#include "state.h"

class Program {

    float const _rate;

    State& _state;

public:

    Program(float rate, State& state)
            : _rate(rate), _state(state) {
    }

    float rate() {
        return _rate;
    }

    void fire(DSSI::cursor cursor);

};
