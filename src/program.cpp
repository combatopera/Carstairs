#include "program.h"

#include <algorithm>
#include <array>

static std::array<int, 10> ENVELOPE {13, 14, 15, 14, 13, 12, 11, 10, 9, 8};

void Program::fire(int noteFrame, int offFrameOrNeg, State& state) const {
    if (offFrameOrNeg < 0) {
        state.setLevel4(ENVELOPE[std::min(noteFrame, int(ENVELOPE.size() - 1))]);
    }
    else {
        state.setLevel4(0);
    }
}
