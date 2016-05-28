#include "program.h"

void Program::fire(int noteFrame, int offFrameOrNeg) const {
    if (offFrameOrNeg < 0) {
        _state.setLevel4(0xf);
    }
    else {
        _state.setLevel4(0);
    }
}
