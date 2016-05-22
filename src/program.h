#pragma once

#include "state.h"
#include "util/util.h"

class Program {

    State& _state;

public:

    Program(State& state)
            : _state(state) {
    }

    void fire(DSSI::cursor cursor);

};
