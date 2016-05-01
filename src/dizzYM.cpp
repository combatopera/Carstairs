#include "dizzYM.h"

#include <alsa/seq_event.h>
#include <ladspa.h>

#include "dssi/port.h"
#include "util.h"

dizzYM::dizzYM(int sampleRate)
        : _sampleRate(sampleRate), _sampleCursor(0), _chip(&_state) {
}

void dizzYM::connect_port(LADSPA_Handle Instance, unsigned long Port, LADSPA_Data *DataLocation) {
    ((dizzYM *) Instance)->_portValPtrs[Port] = DataLocation;
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
        _chip.render(_sampleCursor + limitInBlock).copyTo(_portValPtrs[OUTPUT_PORT_INFO._ordinal] + indexInBlock);
        indexInBlock = limitInBlock;
    }
    _sampleCursor += blockSize;
}
