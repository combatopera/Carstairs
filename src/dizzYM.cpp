#include "dizzYM.h"

#include <dssi.h>

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
        _program(_state), //
        _tone(config, _state), //
        _level(config, _state, _tone), //
        _pcm(config, _state, _level, pcmRate) {
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

void dizzYM::runSynth(DSSI::cursor blockSize, snd_seq_event_t const *events, DSSI::cursor eventCount) {
    auto const refCursor = _pcm.cursor();
    if (0 == refCursor || eventCount) {
        debug("%u -> %u", refCursor, refCursor + blockSize);
    }
    for (DSSI::cursor indexInBlock = 0, eventIndex = 0; indexInBlock < blockSize;) {
        // Consume all events effective at indexInBlock:
        for (; eventIndex < eventCount; ++eventIndex) {
            auto const& event = events[eventIndex];
            auto const eventX = refCursor + event.time.tick;
            if (eventX > _pcm.cursor()) {
                break;
            }
            switch (event.type) {
                case SND_SEQ_EVENT_NOTEON: {
                    auto const& n = event.data.note;
                    _state.noteOn(eventX, n.note, n.velocity);
                    break;
                }
                case SND_SEQ_EVENT_NOTEOFF: {
                    _state.noteOff(eventX, event.data.note.note);
                    break;
                }
            }
        }
        _program.fire(_pcm.cursor());
        // Set limit to sample index of next event, or blockSize if there isn't one in this block:
        auto limitInBlock = eventIndex < eventCount && events[eventIndex].time.tick < blockSize ? events[eventIndex].time.tick : blockSize;
        _pcm.render(refCursor + limitInBlock).copyTo(_portValPtrs.at(_PortInfo._pcm._ordinal) + indexInBlock);
        indexInBlock = limitInBlock;
    }
}
