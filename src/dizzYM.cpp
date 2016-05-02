#include "dizzYM.h"

#include <alsa/seq_event.h>
#include <dssi.h>
#include <ladspa.h>

#include "util.h"

PortInfoEnum::PortInfoEnum(int ord)
        : OUTPUT {ord++, true, true, "Output", 0, 0, 0, DSSI_NONE}, //
        SUSTAIN {ord++, false, false, "Sustain (on/off)",
        LADSPA_HINT_DEFAULT_MINIMUM | LADSPA_HINT_INTEGER | LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE, 0, 1, DSSI_CC(64)}, //
        _values {&OUTPUT, &SUSTAIN}, //
        _count(ord - OUTPUT._ordinal) {
}

dizzYM::dizzYM(int sampleRate)
        : _portValPtrs("_portValPtrs"), _sampleRate(sampleRate), _sampleCursor(0), _chip(&_state) {
    _portValPtrs.setLimit(PortInfo._count);
}

void dizzYM::setPortValPtr(int index, LADSPA_Data *valPtr) {
    _portValPtrs.put(index, valPtr);
}

void dizzYM::reset() {
    _sampleCursor = 0;
    _state._noteOn = -1;
    _state._noteOff = -1;
    _state._velocity = 0;
}

void dizzYM::runSynth(unsigned long blockSize, snd_seq_event_t *events, unsigned long eventCount) {
    for (unsigned long indexInBlock = 0, eventIndex = 0; indexInBlock < blockSize;) {
        // Consume all events effective at indexInBlock:
        for (; eventIndex < eventCount && events[eventIndex].time.tick <= indexInBlock; ++eventIndex) {
            switch (events[eventIndex].type) {
                case SND_SEQ_EVENT_NOTEON: {
                    snd_seq_ev_note_t *n = &events[eventIndex].data.note;
                    _state._midiNote = n->note;
                    _state._noteOn = _sampleCursor + events[eventIndex].time.tick;
                    _state._noteOff = -1;
                    _state._velocity = n->velocity;
                    break;
                }
                case SND_SEQ_EVENT_NOTEOFF: {
                    if (_state._midiNote == events[eventIndex].data.note.note) {
                        _state._noteOff = _sampleCursor + events[eventIndex].time.tick;
                    }
                    break;
                }
            }
        }
        // Set limit to sample index of next event, or blockSize if there isn't one in this block:
        unsigned long limitInBlock = eventIndex < eventCount && events[eventIndex].time.tick < blockSize ? events[eventIndex].time.tick : blockSize;
        _chip.render(_sampleCursor + limitInBlock).copyTo(_portValPtrs.at(PortInfo.OUTPUT._ordinal) + indexInBlock);
        indexInBlock = limitInBlock;
    }
    _sampleCursor += blockSize;
}
