#include "dizzYM.h"

#include <dssi.h>
#include <algorithm>
#include <cmath>

#include "node.h"
#include "util/util.h"

PortInfoEnum::PortInfoEnum(Config const& config, sizex ord)
        : _pcm {ord++, true, true, "PCM", 0, //
                0, 0, DSSI_NONE}, //
        _alpha {ord++, false, false, "alpha", LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_MIDDLE, //
                -1, 1, DSSI_CC(config._alphaCC)}, //
        _beta {ord++, false, false, "beta", LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_MIDDLE, //
                -1, 1, DSSI_CC(config._betaCC)}, //
        _values {&_pcm, ord} {
    debug("Constructed the PortInfoEnum.");
}

dizzYM::dizzYM(Config const& config, PortInfoEnum const& PortInfo, int const pcmRate)
        : _PortInfo(PortInfo), _portValPtrs("_portValPtrs", PortInfo._values._n), //
        _state(config), //
        _program(config), //
        _tone(config, _state), //
        _level(config, _state, _tone), //
        _pcm(config, _state, _level, pcmRate), //
        _pcmRate(pcmRate) {
}

void dizzYM::setPortValPtr(sizex index, LADSPA_Data *valPtr) {
    _portValPtrs.put(index, valPtr);
}

void dizzYM::start() {
    _state.reset();
    // TODO: Reliably start all nodes.
    _tone.start();
    _level.start();
    _pcm.start();
}

// FIXME LATER: This mechanism should not be affected by the PCM rate.
DSSI::cursor dizzYM::getProgramEventX(DSSI::cursor voidX) const {
    auto const onOrMax = _state.onOrMax();
    if (DSSI::CURSOR_MAX != onOrMax) {
        // If the logical cursor is a fraction, it affects the next physical cursor:
        return onOrMax + DSSI::cursor(ceil(double(_state.programEventIndex()) / _program.rate() * _pcmRate));
    }
    return voidX; // Not in this block.
}

void dizzYM::runSynth(DSSI::cursor blockSize, snd_seq_event_t const *events, DSSI::cursor eventCount) {
    auto const refCursor = _pcm.cursor();
    auto const voidX = refCursor + blockSize;
    if (0 == refCursor || eventCount) {
        debug("%u -> %u", refCursor, voidX);
    }
    auto destPtr = _portValPtrs.at(_PortInfo._pcm.ordinal());
    auto const destEnd = destPtr + blockSize;
    sizex eventIndex = 0;
    while (destPtr < destEnd) {
        DSSI::cursor targetX;
        while (true) {
            auto const hostEventX = eventIndex < eventCount ? refCursor + events[eventIndex].time.tick : voidX;
            auto const programEventX = getProgramEventX(voidX);
            targetX = std::min(programEventX, hostEventX);
            if (_pcm.cursor() < targetX) {
                break; // In particular when targetX is voidX.
            }
            if (hostEventX <= programEventX) { // Prioritise host events if equal.
                auto const& event = events[eventIndex++];
                switch (event.type) {
                    case SND_SEQ_EVENT_NOTEON: {
                        _program.refresh();
                        auto const& n = event.data.note;
                        _state.noteOn(hostEventX, n.note, n.velocity);
                        break;
                    }
                    case SND_SEQ_EVENT_NOTEOFF: {
                        _state.noteOff(hostEventX, event.data.note.note);
                        break;
                    }
                }
            }
            else {
                _state.fire(_program);
            }
        }
        // It's possible for both the next host and program events to be beyond the block, so clamp:
        auto const block = _pcm.render(std::min(targetX, voidX));
        block.copyTo(destPtr);
        destPtr += block.limit();
    }
}
