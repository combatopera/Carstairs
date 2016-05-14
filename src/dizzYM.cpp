#include "dizzYM.h"

#include <alsa/seq_event.h>
#include <dssi.h>
#include <ladspa.h>

#include "buf.h"
#include "node.h"
#include "util/util.h"

PortInfoEnum::PortInfoEnum(index_t ord)
        : _output {ord++, true, true, "Output", 0, 0, 0, DSSI_NONE}, //
        _sustain {ord++, false, false, "Sustain (on/off)",
        LADSPA_HINT_DEFAULT_MINIMUM | LADSPA_HINT_INTEGER | LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE, 0, 1, DSSI_CC(64)}, //
        _values {&_output, ord} {
    debug("Constructed the PortInfoEnum.");
}

dizzYM::dizzYM(float pcmRate)
        : _portValPtrs("_portValPtrs"), //
        _sampleCursor(INITIAL_SAMPLE_CURSOR), //
        _config(pcmRate), //
        _state(&_config), //
        _program(&_state), //
        _tone(&_config, &_state), //
        _level(&_state, &_tone), //
        _chip(&_config, &_state, &_level) {
    _portValPtrs.setLimit(PortInfo._values._n);
}

void dizzYM::setPortValPtr(int index, LADSPA_Data *valPtr) {
    _portValPtrs.put(index, valPtr);
}

void dizzYM::reset() {
    _sampleCursor = INITIAL_SAMPLE_CURSOR;
    _state.reset();
    _tone.reset();
    _chip.reset();
}

void dizzYM::runSynth(cursor_t blockSize, snd_seq_event_t *events, cursor_t eventCount) {
    if (0 == _sampleCursor || eventCount) {
        debug("%d -> %d", _sampleCursor, _sampleCursor + blockSize);
    }
    for (cursor_t indexInBlock = 0, eventIndex = 0; indexInBlock < blockSize;) {
        // Consume all events effective at indexInBlock:
        for (; eventIndex < eventCount && events[eventIndex].time.tick <= indexInBlock; ++eventIndex) {
            switch (events[eventIndex].type) {
                case SND_SEQ_EVENT_NOTEON: {
                    snd_seq_ev_note_t *n = &events[eventIndex].data.note;
                    _state.noteOn(_sampleCursor + events[eventIndex].time.tick, n->note, n->velocity);
                    break;
                }
                case SND_SEQ_EVENT_NOTEOFF: {
                    _state.noteOff(_sampleCursor + events[eventIndex].time.tick, events[eventIndex].data.note.note);
                    break;
                }
            }
        }
        _program.fire(_sampleCursor + indexInBlock);
        // Set limit to sample index of next event, or blockSize if there isn't one in this block:
        cursor_t limitInBlock = eventIndex < eventCount && events[eventIndex].time.tick < blockSize ? events[eventIndex].time.tick : blockSize;
        _chip.render(_sampleCursor + limitInBlock).copyTo(_portValPtrs.at(PortInfo._output._ordinal) + indexInBlock);
        indexInBlock = limitInBlock;
    }
    _sampleCursor += blockSize;
}
