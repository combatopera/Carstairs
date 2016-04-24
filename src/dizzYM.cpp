#include "dizzYM.h"

#include <alsa/seq_event.h>
#include <ladspa.h>
#include <math.h>
#include <stddef.h>
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
    for (int midiNote = 0; midiNote < MIDI_NOTE_COUNT; ++midiNote) {
        plugin->_notes[midiNote].reset();
    }
}

void Note::reset() {
    _on = _off = -1;
    _velocity = 0;
}

void Note::on(unsigned long on, int velocity) {
    _on = on;
    _off = -1;
    _velocity = velocity;
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

void dizzYM::runSynth(unsigned long sampleCount, snd_seq_event_t *events, unsigned long eventCount) {
    LADSPA_Data *output = _portValPtrs[OUTPUT_PORT_INFO._ordinal];
    for (unsigned long sampleIndex = 0, eventIndex = 0; sampleIndex < sampleCount;) {
        while (eventIndex < eventCount && events[eventIndex].time.tick <= sampleIndex) {
            switch (events[eventIndex].type) {
                case SND_SEQ_EVENT_NOTEON: {
                    snd_seq_ev_note_t *n = &events[eventIndex].data.note;
                    _notes[n->note].on(_sampleCursor + events[eventIndex].time.tick, n->velocity);
                    break;
                }
                case SND_SEQ_EVENT_NOTEOFF: {
                    _notes[events[eventIndex].data.note.note]._off = _sampleCursor + events[eventIndex].time.tick;
                    break;
                }
            }
            ++eventIndex;
        }
        unsigned long count;
        if (eventIndex < eventCount && events[eventIndex].time.tick < sampleCount) {
            count = events[eventIndex].time.tick - sampleIndex;
        }
        else {
            count = sampleCount - sampleIndex;
        }
        for (unsigned long k = 0; k < count; ++k) {
            output[sampleIndex + k] = 0;
        }
        for (int midiNote = 0; midiNote < MIDI_NOTE_COUNT; ++midiNote) {
            if (_notes[midiNote]._on >= 0) {
                addSamples(midiNote, sampleIndex, count);
            }
        }
        sampleIndex += count;
    }
    _sampleCursor += sampleCount;
}

void dizzYM::addSamples(int midiNote, unsigned long offset, unsigned long count) {
#ifdef DEBUG_dizzYM
    std::cerr << "dizzYM::addSamples(" << midiNote << ", " << offset << ", " << count << "): on " << _notes[midiNote]._on << ", off " << _notes[midiNote]._off
            << ", size " << _sizes[midiNote] << ", start " << _sampleCursor + offset << std::endl;
#endif
    LADSPA_Data *output = _portValPtrs[OUTPUT_PORT_INFO._ordinal];
    bool sustain = *_portValPtrs[SUSTAIN_PORT_INFO._ordinal];
    if (_notes[midiNote]._on < 0) {
        return;
    }
    unsigned long absOn = (unsigned long) (_notes[midiNote]._on);
    unsigned long absStart = _sampleCursor + offset;
    if (absStart < absOn) {
        return;
    }
    LADSPA_Data *noiseBurst = _noiseBursts[midiNote];
    if (absStart == absOn) {
        for (size_t i = 0; i < _sizes[midiNote] + 1; ++i) {
            noiseBurst[i] = (float(rand()) / float(RAND_MAX)) * 2 - 1;
        }
    }
    size_t i, s;
    float normVel = float(_notes[midiNote]._velocity) / 127;
    for (i = 0, s = absStart - absOn; i < count; ++i, ++s) {
        float gain(normVel);
        if (!sustain && _notes[midiNote]._off >= 0 && (unsigned long) (_notes[midiNote]._off) < i + absStart) {
            unsigned long release = (unsigned long) (1 + (0.01 * _sampleRate));
            unsigned long dist = i + absStart - _notes[midiNote]._off;
            if (dist > release) {
                _notes[midiNote]._on = -1;
                break;
            }
            gain = gain * float(release - dist) / float(release);
        }
        size_t sz = _sizes[midiNote];
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
        output[offset + i] += gain * sample;
    }
}
