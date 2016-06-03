#include "carstairs.h"

#include <dssi.h>
#include <algorithm>
#include <cmath>

#include "node.h"
#include "util/util.h"

namespace {
Log const LOG(__FILE__);
}

PortInfoEnum::PortInfoEnum(Config const& config, sizex ord)
        : PCM {ord++, true, true, "PCM", 0, //
                0, 0, DSSI_NONE}, //
        ALPHA {ord++, false, false, "alpha", LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_MIDDLE, //
                -1, 1, DSSI_CC(config._alphaCC)}, //
        BETA {ord++, false, false, "beta", LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_MIDDLE, //
                -1, 1, DSSI_CC(config._betaCC)}, //
        _values {&PCM, ord} {
    CARSTAIRS_DEBUG("Constructed the PortInfoEnum.");
}

Carstairs::Carstairs(Config const& config, PortInfoEnum const& PortInfo, Python const& python, Programs const& programs, int const pcmRate)
        : _PortInfo(PortInfo), _portValPtrs("_portValPtrs", PortInfo.values().length), //
        _state(config), //
        _loader(config, python, programs), //
        _tone(config, _state), //
        _level(config, _state, _tone), //
        _pcm(config, _state, _level, pcmRate), //
        _pcmRate(pcmRate) {
}

void Carstairs::setPortValPtr(sizex index, LADSPA_Data *valPtr) {
    _portValPtrs.put(index, valPtr);
}

void Carstairs::start() {
    _state.reset();
    // TODO: Reliably start all nodes.
    _tone.start();
    _level.start();
    _pcm.start();
}

// FIXME LATER: This mechanism should not be affected by the PCM rate.
DSSI::cursor Carstairs::getProgramEventX(DSSI::cursor voidX) const {
    auto const onOrMax = _state.onOrMax();
    if (DSSI::CURSOR_MAX != onOrMax) {
        // If the logical cursor is a fraction, it affects the next physical cursor:
        return onOrMax + DSSI::cursor(ceil(double(_state.programEventIndex()) / _loader.currentProgram().rate() * _pcmRate));
    }
    return voidX; // Not in this block.
}

void Carstairs::runSynth(DSSI::cursor blockSize, snd_seq_event_t const *events, DSSI::cursor eventCount) {
    auto const refCursor = _pcm.cursor();
    auto const voidX = refCursor + blockSize;
    if (0 == refCursor || eventCount) {
        CARSTAIRS_TRACE("%lu -> %lu", refCursor, voidX);
    }
    auto destPtr = _portValPtrs.at(_PortInfo.PCM.ordinal());
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
                        _loader.refresh(0);
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
                _state.fire(_loader.currentProgram());
            }
        }
        // It's possible for both the next host and program events to be beyond the block, so clamp:
        auto const block = _pcm.render(std::min(targetX, voidX));
        block.copyTo(destPtr);
        destPtr += block.limit();
    }
}
