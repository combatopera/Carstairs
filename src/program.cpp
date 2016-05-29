#include "program.h"

void Program::fire(int noteFrame, int offFrameOrNeg, State& state) const {
    if (offFrameOrNeg < 0) {
        state.setLevel4(0xf);
    }
    else {
        state.setLevel4(0);
    }
}
