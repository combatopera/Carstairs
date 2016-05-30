#include "program.h"

void Program::fire(int noteFrame, int offFrameOrNeg, State& state) const {
    if (!noteFrame) {
        state.setLevel4(13); // Use half the available amp.
    }
    if (_module) {
        if (offFrameOrNeg < 0) {
            _module.getAttr("on").callVoid("(i)", noteFrame);
        }
        else {
            _module.getAttr("off").callVoid("ii", noteFrame, offFrameOrNeg);
        }
        auto const chan = _module.getAttr("chan");
        if (chan) {
            auto const level = chan.getAttr("level");
            if (level) {
                state.setLevel4(level.numberRoundToInt());
            }
        }
    }
}
