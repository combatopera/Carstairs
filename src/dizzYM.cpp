#include "dizzYM.h"

#include <alsa/seq_event.h>
#include <ladspa.h>

#include "dssi/port.h"
#include "util.h"

#ifdef DEBUG_dizzYM
#include <iostream>
#endif

dizzYM::dizzYM(int sampleRate)
        : _sampleRate(sampleRate), _sampleCursor(0) {
}

dizzYM::~dizzYM() {
}

LADSPA_Handle dizzYM::instantiate(const LADSPA_Descriptor *Descriptor, unsigned long SampleRate) {
    return new dizzYM((int) SampleRate);
}

void dizzYM::cleanup(LADSPA_Handle Instance) {
    delete (dizzYM *) Instance;
}

void dizzYM::connect_port(LADSPA_Handle Instance, unsigned long Port, LADSPA_Data *DataLocation) {
    ((dizzYM *) Instance)->_portValPtrs[Port] = DataLocation;
}

void dizzYM::activate(LADSPA_Handle Instance) {
    dizzYM *plugin = (dizzYM *) Instance;
    plugin->_sampleCursor = 0;
    plugin->_noteOn = -1;
    plugin->_noteOff = -1;
    plugin->_velocity = 0;
}

int dizzYM::get_midi_controller_for_port(LADSPA_Handle, unsigned long Port) {
    return PORT_INFOS[Port]->_controllers;
}

void dizzYM::run(LADSPA_Handle Instance, unsigned long SampleCount) {
    ((dizzYM *) Instance)->runSynth(SampleCount, 0, 0);
}

void dizzYM::run_synth(LADSPA_Handle Instance, unsigned long SampleCount, snd_seq_event_t *Events, unsigned long EventCount) {
    ((dizzYM *) Instance)->runSynth(SampleCount, Events, EventCount);
}

void dizzYM::runSynth(unsigned long blockSize, snd_seq_event_t *events, unsigned long eventCount) {
    for (unsigned long indexInBlock = 0, eventIndex = 0; indexInBlock < blockSize;) {
        // Consume all events effective at indexInBlock:
        for (; eventIndex < eventCount && events[eventIndex].time.tick <= indexInBlock; ++eventIndex) {
            switch (events[eventIndex].type) {
                case SND_SEQ_EVENT_NOTEON: {
                    snd_seq_ev_note_t *n = &events[eventIndex].data.note;
                    _midiNote = n->note;
                    _noteOn = _sampleCursor + events[eventIndex].time.tick;
                    _noteOff = -1;
                    _velocity = n->velocity;
                    break;
                }
                case SND_SEQ_EVENT_NOTEOFF: {
                    if (_midiNote == events[eventIndex].data.note.note) {
                        _noteOff = _sampleCursor + events[eventIndex].time.tick;
                    }
                    break;
                }
            }
        }
        // Set limit to sample index of next event, or blockSize if there isn't one in this block:
        unsigned long limitInBlock = eventIndex < eventCount && events[eventIndex].time.tick < blockSize ? events[eventIndex].time.tick : blockSize;
        putSamples(indexInBlock, limitInBlock);
        indexInBlock = limitInBlock;
    }
    _sampleCursor += blockSize;
}

void dizzYM::putSamples(unsigned long indexInBlock, unsigned long limitInBlock) {
#ifdef DEBUG_dizzYM
    std::cerr << "dizzYM::putSamples(" << indexInBlock << ", " << limitInBlock << "): on " << _noteOn << ", off " << _noteOff << ", start "
            << _sampleCursor + indexInBlock << std::endl;
#endif
    copy(_chip.render(_sampleCursor + limitInBlock), _portValPtrs[OUTPUT_PORT_INFO._ordinal] + indexInBlock, limitInBlock - indexInBlock);
}
