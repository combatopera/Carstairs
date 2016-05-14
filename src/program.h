#pragma once

#include "state.h"
#include "util/util.h"

class Program {

    State * const _state;

public:

    Program(State *state)
            : _state(state) {
    }

    void fire(cursor_t cursor);

};
