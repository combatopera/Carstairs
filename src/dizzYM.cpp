#include "dizzYM.h"

#include <alsa/seq_event.h>
#include <dssi.h>
#include <ladspa.h>
#include <math.h>
#include <stddef.h>
#include <cstdlib>

#include "port.h"

#ifdef DEBUG_dizzYM
#include <iostream>
#endif

const Port dizzYM::PORTS[PortCount] = { //
        Port("Output", LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO, 0, 0, 0), //
        Port("Sustain (on/off)", LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL,
                LADSPA_HINT_DEFAULT_MINIMUM | LADSPA_HINT_INTEGER | LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE, 0, 1), //
        };

dizzYM::dizzYM(int sampleRate)
        : _output(0), _sustain(0), _sampleRate(sampleRate), _sampleCursor(0) {
    for (int midiNote = 0; midiNote < Notes; ++midiNote) {
        float frequency = 440 * powf(2, float(midiNote - 69) / 12);
        _sizes[midiNote] = float(sampleRate) / frequency;
        _wavetable[midiNote] = new float[int(_sizes[midiNote]) + 1];
    }
}

dizzYM::~dizzYM() {
    for (int midiNote = 0; midiNote < Notes; ++midiNote) {
        delete[] _wavetable[midiNote];
    }
}

LADSPA_Handle dizzYM::instantiate(const LADSPA_Descriptor *Descriptor, unsigned long SampleRate) {
    return new dizzYM((int) SampleRate);
}

void dizzYM::cleanup(LADSPA_Handle Instance) {
    delete (dizzYM *) Instance;
}

void dizzYM::connect_port(LADSPA_Handle Instance, unsigned long Port, LADSPA_Data *DataLocation) {
    dizzYM *plugin = (dizzYM *) Instance;
    float **ports[PortCount] = {&plugin->_output, &plugin->_sustain};
    *ports[Port] = (float *) DataLocation;
}

void dizzYM::activate(LADSPA_Handle Instance) {
    dizzYM *plugin = (dizzYM *) Instance;
    plugin->_sampleCursor = 0;
    for (int midiNote = 0; midiNote < Notes; ++midiNote) {
        plugin->_ons[midiNote] = -1;
        plugin->_offs[midiNote] = -1;
        plugin->_velocities[midiNote] = 0;
    }
}

int dizzYM::get_midi_controller_for_port(LADSPA_Handle, unsigned long Port) {
    int controllers[PortCount] = {DSSI_NONE, DSSI_CC(64)};
    return controllers[Port];
}

void dizzYM::run(LADSPA_Handle Instance, unsigned long SampleCount) {
    ((dizzYM *) Instance)->runSynth(SampleCount, 0, 0);
}

void dizzYM::run_synth(LADSPA_Handle Instance, unsigned long SampleCount, snd_seq_event_t *Events, unsigned long EventCount) {
    ((dizzYM *) Instance)->runSynth(SampleCount, Events, EventCount);
}

void dizzYM::runSynth(unsigned long sampleCount, snd_seq_event_t *events, unsigned long eventCount) {
    for (unsigned long sampleIndex = 0, eventIndex = 0; sampleIndex < sampleCount;) {
        while (eventIndex < eventCount && sampleIndex >= events[eventIndex].time.tick) {
            switch (events[eventIndex].type) {
                case SND_SEQ_EVENT_NOTEON: {
                    snd_seq_ev_note_t *n = &events[eventIndex].data.note;
                    if (n->velocity > 0) {
                        _ons[n->note] = _sampleCursor + events[eventIndex].time.tick;
                        _offs[n->note] = -1;
                        _velocities[n->note] = n->velocity;
                    }
                    break;
                }
                case SND_SEQ_EVENT_NOTEOFF: {
                    _offs[events[eventIndex].data.note.note] = _sampleCursor + events[eventIndex].time.tick;
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
            _output[sampleIndex + k] = 0;
        }
        for (int midiNote = 0; midiNote < Notes; ++midiNote) {
            if (_ons[midiNote] >= 0) {
                addSamples(midiNote, sampleIndex, count);
            }
        }
        sampleIndex += count;
    }
    _sampleCursor += sampleCount;
}

void dizzYM::addSamples(int midiNote, unsigned long offset, unsigned long count) {
#ifdef DEBUG_dizzYM
    std::cerr << "dizzYM::addSamples(" << midiNote << ", " << offset << ", " << count << "): on " << _ons[midiNote] << ", off " << _offs[midiNote] << ", size "
            << _sizes[midiNote] << ", start " << _sampleCursor + offset << std::endl;
#endif
    if (_ons[midiNote] < 0) {
        return;
    }
    unsigned long on = (unsigned long) (_ons[midiNote]);
    unsigned long start = _sampleCursor + offset;
    if (start < on) {
        return;
    }
    if (start == on) {
        for (size_t i = 0; i <= unsigned(_sizes[midiNote]); ++i) {
            _wavetable[midiNote][i] = (float(rand()) / float(RAND_MAX)) * 2 - 1;
        }
    }
    size_t i, s;
    float vgain = (float) (_velocities[midiNote]) / 127.0f;
    for (i = 0, s = start - on; i < count; ++i, ++s) {
        float gain(vgain);
        if ((!_sustain || !*_sustain) && _offs[midiNote] >= 0 && (unsigned long) (_offs[midiNote]) < i + start) {
            unsigned long release = (unsigned long) (1 + (0.01 * _sampleRate));
            unsigned long dist = i + start - _offs[midiNote];
            if (dist > release) {
                _ons[midiNote] = -1;
                break;
            }
            gain = gain * float(release - dist) / float(release);
        }
        size_t sz = int(_sizes[midiNote]);
        bool decay = (s > sz);
        size_t index = (s % int(sz));
        float sample = _wavetable[midiNote][index];
        if (decay) {
            if (index == 0) {
                sample += _wavetable[midiNote][sz - 1];
            }
            else {
                sample += _wavetable[midiNote][index - 1];
            }
            sample /= 2;
            _wavetable[midiNote][index] = sample;
        }
        _output[offset + i] += gain * sample;
    }
}
