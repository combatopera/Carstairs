#include "dizzYM.h"

#include <alsa/seq_event.h>
#include <ladspa.h>
#include <math.h>
#include <stddef.h>
#include <string.h>
#include <cstdlib>

#include "port.h"

#ifdef DEBUG_dizzYM
#include <iostream>
#endif

dizzYM::dizzYM(int sampleRate)
        : _sampleRate(sampleRate), _sampleCursor(0) {
    for (int midiNote = 0; midiNote < MIDI_NOTE_COUNT; ++midiNote) {
        float frequency = 440 * powf(2, float(midiNote - 69) / 12);
        _sizes[midiNote] = unsigned(float(sampleRate) / frequency);
        _noiseBursts[midiNote] = new LADSPA_Data[_sizes[midiNote] + 1];
    }
}

dizzYM::~dizzYM() {
    for (int midiNote = 0; midiNote < MIDI_NOTE_COUNT; ++midiNote) {
        delete[] _noiseBursts[midiNote];
    }
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
    LADSPA_Data *output = _portValPtrs[OUTPUT_PORT_INFO._ordinal];
    memset(output, 0, blockSize * sizeof *output); // Not portable.
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
                    _noteOff = _sampleCursor + events[eventIndex].time.tick;
                    break;
                }
            }
        }
        // Set limit to sample index of next event, or blockSize if there isn't one in this block:
        unsigned long limitInBlock = eventIndex < eventCount && events[eventIndex].time.tick < blockSize ? events[eventIndex].time.tick : blockSize;
        if (_noteOn >= 0) {
            addSamples(indexInBlock, limitInBlock - indexInBlock);
        }
        indexInBlock = limitInBlock;
    }
    _sampleCursor += blockSize;
}

void dizzYM::addSamples(unsigned long indexInBlock, unsigned long sampleCount) {
#ifdef DEBUG_dizzYM
    std::cerr << "dizzYM::addSamples(" << _midiNote << ", " << indexInBlock << ", " << sampleCount << "): on " << _noteOn << ", off " << _noteOff << ", size "
            << _sizes[_midiNote] << ", start " << _sampleCursor + indexInBlock << std::endl;
#endif
    LADSPA_Data *output = _portValPtrs[OUTPUT_PORT_INFO._ordinal] + indexInBlock;
    bool sustain = *_portValPtrs[SUSTAIN_PORT_INFO._ordinal];
    unsigned long absOn = (unsigned long) _noteOn;
    unsigned long absStart = _sampleCursor + indexInBlock;
    if (absStart < absOn) {
        return;
    }
    LADSPA_Data *noiseBurst = _noiseBursts[_midiNote];
    if (absStart == absOn) {
        for (size_t i = 0; i < _sizes[_midiNote] + 1; ++i) {
            noiseBurst[i] = (float(rand()) / float(RAND_MAX)) * 2 - 1;
        }
    }
    size_t i, s;
    float normVel = float(_velocity) / 127;
    for (i = 0, s = absStart - absOn; i < sampleCount; ++i, ++s) {
        float gain(normVel);
        if (!sustain && _noteOff >= 0 && (unsigned long) (_noteOff) < i + absStart) {
            unsigned long release = (unsigned long) (1 + (0.01 * _sampleRate));
            unsigned long dist = i + absStart - _noteOff;
            if (dist > release) {
                _noteOn = -1;
                break;
            }
            gain = gain * float(release - dist) / float(release);
        }
        size_t sz = _sizes[_midiNote];
        bool decay = s > sz;
        size_t index = s % sz;
        LADSPA_Data sample = noiseBurst[index];
        if (decay) {
            if (index == 0) {
                sample += noiseBurst[sz - 1];
            }
            else {
                sample += noiseBurst[index - 1];
            }
            sample /= 2;
            noiseBurst[index] = sample;
        }
        output[i] += gain * sample;
    }
}
