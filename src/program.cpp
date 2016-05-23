#include "program.h"

void Program::fire(DSSI::cursor cursor) {
    auto onOrMax = _state.onOrMax();
    if (DSSI::CURSOR_MAX == onOrMax || cursor < onOrMax) {
        _state.setLevel4(0); // Never been on or not yet on.
    }
    else {
        auto offOrMax = _state.offOrMax();
        if (DSSI::CURSOR_MAX == offOrMax || cursor < offOrMax) {
            _state.setLevel4(0xf); // Not yet off.
        }
        else {
            _state.setLevel4(0); // Off.
        }
    }
}
