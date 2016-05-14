#include "program.h"

#include "state.h"
#include "util/util.h"

void Program::fire(cursor_t cursor) {
    cursor_t onOrMax = _state->onOrMax();
    if (CURSOR_MAX == onOrMax || cursor < onOrMax) {
        _state->setLevel4(0); // Never been on or not yet on.
    }
    else {
        cursor_t offOrMax = _state->offOrMax();
        if (CURSOR_MAX == offOrMax || cursor < offOrMax) {
            _state->setLevel4(0xf); // Not yet off.
        }
        else {
            _state->setLevel4(0); // Off.
        }
    }
}
