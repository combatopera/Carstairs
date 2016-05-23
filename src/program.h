#pragma once

#include "dssi/plugin.h"
#include "state.h"

class Program {

    State& _state;

public:

    Program(State& state)
            : _state(state) {
    }

    void fire(DSSI::cursor cursor);

};
