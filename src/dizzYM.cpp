#include <alsa/seq_event.h>
#include <dssi.h>
#include <ladspa.h>
#include <math.h>
#include <stddef.h>
#include <stdlib.h>

#ifdef DEBUG_dizzYM
#include <iostream>
#endif

class dizzYM {

    dizzYM(int sampleRate);

    ~dizzYM();

    enum {

        OutputPort = 0, Sustain = 1, PortCount = 2

    };

    enum {

        Notes = 128

    };

    static const char * const PortNames[PortCount];

    static const LADSPA_PortDescriptor PortDescriptors[PortCount];

    static const LADSPA_PortRangeHint PortRangeHints[PortCount];

    static const LADSPA_Properties Properties;

    static const LADSPA_Descriptor ladspaDescriptor;

    static LADSPA_Handle instantiate(const LADSPA_Descriptor *, unsigned long);
    static void connect_port(LADSPA_Handle, unsigned long, LADSPA_Data *);
    static void activate(LADSPA_Handle);
    static void run(LADSPA_Handle, unsigned long);
    static void deactivate(LADSPA_Handle);
    static void cleanup(LADSPA_Handle);
    static const DSSI_Program_Descriptor *getProgram(LADSPA_Handle, unsigned long);
    static void selectProgram(LADSPA_Handle, unsigned long, unsigned long);
    static int get_midi_controller_for_port(LADSPA_Handle, unsigned long);
    static void run_synth(LADSPA_Handle, unsigned long, snd_seq_event_t *, unsigned long);

    void runImpl(unsigned long, snd_seq_event_t *, unsigned long);
    void addSamples(int, unsigned long, unsigned long);

    float *_output;

    float *_sustain;

    int _sampleRate;

    long _blockStart;

    long _ons[Notes];

    long _offs[Notes];

    int _velocities[Notes];

    float *_wavetable[Notes];

    float _sizes[Notes];

public:

    static const DSSI_Descriptor dssiDescriptor;

};

const char * const dizzYM::PortNames[PortCount] = { //
        "Output", //
                "Sustain (on/off)", //
        };

const LADSPA_PortDescriptor dizzYM::PortDescriptors[PortCount] = { //
        LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO, //
                LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL, //
        };

const LADSPA_PortRangeHint dizzYM::PortRangeHints[PortCount] = { //
        {0, 0, 0}, //
                {LADSPA_HINT_DEFAULT_MINIMUM | LADSPA_HINT_INTEGER | LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE, 0, 1}, //
        };

const LADSPA_Properties dizzYM::Properties = LADSPA_PROPERTY_HARD_RT_CAPABLE;

const LADSPA_Descriptor dizzYM::ladspaDescriptor = { //
        0, // UniqueID
                "dizzYM", // Label
                Properties, //
                "YM2149", // Name
                "Andrzej Cichocki", // Maker
                "Andrzej Cichocki", // Copyright
                PortCount, //
                PortDescriptors, //
                PortNames, //
                PortRangeHints, //
                0, // ImplementationData
                instantiate, //
                connect_port, //
                activate, //
                run, //
                0, // run_adding
                0, // set_run_adding_gain
                deactivate, //
                cleanup, //
        };

const DSSI_Descriptor dizzYM::dssiDescriptor = { //
        1, // API version, must be 1.
                &ladspaDescriptor, //
                0, // configure()
                0, // get_program()
                0, // select_program()
                get_midi_controller_for_port, //
                run_synth, //
                0, // run_synth_adding()
                0, // run_multiple_synths()
                0, // run_multiple_synths_adding()
        };

dizzYM::dizzYM(int sampleRate)
        : _output(0), _sustain(0), _sampleRate(sampleRate), _blockStart(0) {
    for (int midiNote = 0; midiNote < Notes; ++midiNote) {
        float frequency = 440 * powf(2, (midiNote - 69) / 12.f);
        _sizes[midiNote] = sampleRate / frequency;
        _wavetable[midiNote] = new float[int(_sizes[midiNote]) + 1];
    }
}

dizzYM::~dizzYM() {
    for (int midiNote = 0; midiNote < Notes; ++midiNote) {
        delete[] _wavetable[midiNote];
    }
}

LADSPA_Handle dizzYM::instantiate(const LADSPA_Descriptor *Descriptor, unsigned long SampleRate) {
    return new dizzYM(SampleRate);
}

void dizzYM::connect_port(LADSPA_Handle handle, unsigned long port, LADSPA_Data *location) {
    dizzYM *plugin = (dizzYM *) handle;
    float **ports[PortCount] = {&plugin->_output, &plugin->_sustain};
    *ports[port] = (float *) location;
}

void dizzYM::activate(LADSPA_Handle handle) {
    dizzYM *plugin = (dizzYM *) handle;
    plugin->_blockStart = 0;
    for (int midiNote = 0; midiNote < Notes; ++midiNote) {
        plugin->_ons[midiNote] = -1;
        plugin->_offs[midiNote] = -1;
        plugin->_velocities[midiNote] = 0;
    }
}

void dizzYM::run(LADSPA_Handle handle, unsigned long samples) {
    run_synth(handle, samples, 0, 0);
}

void dizzYM::deactivate(LADSPA_Handle handle) {
    activate(handle); // both functions just reset the plugin
}

void dizzYM::cleanup(LADSPA_Handle handle) {
    delete (dizzYM *) handle;
}

int dizzYM::get_midi_controller_for_port(LADSPA_Handle, unsigned long port) {
    int controllers[PortCount] = {DSSI_NONE, DSSI_CC(64)};
    return controllers[port];
}

void dizzYM::run_synth(LADSPA_Handle handle, unsigned long samples, snd_seq_event_t *events, unsigned long eventCount) {
    ((dizzYM *) handle)->runImpl(samples, events, eventCount);
}

void dizzYM::runImpl(unsigned long sampleCount, snd_seq_event_t *events, unsigned long eventCount) {
    for (unsigned long pos = 0, eventPos = 0; pos < sampleCount;) {
        while (eventPos < eventCount && pos >= events[eventPos].time.tick) {
            switch (events[eventPos].type) {
                case SND_SEQ_EVENT_NOTEON: {
                    snd_seq_ev_note_t *n = &events[eventPos].data.note;
                    if (n->velocity > 0) {
                        _ons[n->note] = _blockStart + events[eventPos].time.tick;
                        _offs[n->note] = -1;
                        _velocities[n->note] = n->velocity;
                    }
                    break;
                }
                case SND_SEQ_EVENT_NOTEOFF: {
                    _offs[events[eventPos].data.note.note] = _blockStart + events[eventPos].time.tick;
                    break;
                }
            }
            ++eventPos;
        }
        unsigned long count = sampleCount - pos;
        if (eventPos < eventCount && events[eventPos].time.tick < sampleCount) {
            count = events[eventPos].time.tick - pos;
        }
        for (unsigned long k = 0; k < count; ++k) {
            _output[pos + k] = 0;
        }
        for (int midiNote = 0; midiNote < Notes; ++midiNote) {
            if (_ons[midiNote] >= 0) {
                addSamples(midiNote, pos, count);
            }
        }
        pos += count;
    }
    _blockStart += sampleCount;
}

void dizzYM::addSamples(int voice, unsigned long offset, unsigned long count) {
#ifdef DEBUG_dizzYM
    std::cerr << "dizzYM::addSamples(" << voice << ", " << offset << ", " << count << "): on " << _ons[voice] << ", off " << _offs[voice] << ", size "
            << _sizes[voice] << ", start " << _blockStart + offset << std::endl;
#endif
    if (_ons[voice] < 0) {
        return;
    }
    unsigned long on = (unsigned long) (_ons[voice]);
    unsigned long start = _blockStart + offset;
    if (start < on) {
        return;
    }
    if (start == on) {
        for (size_t i = 0; i <= unsigned(_sizes[voice]); ++i) {
            _wavetable[voice][i] = (float(rand()) / float(RAND_MAX)) * 2 - 1;
        }
    }
    size_t i, s;
    float vgain = (float) (_velocities[voice]) / 127.0f;
    for (i = 0, s = start - on; i < count; ++i, ++s) {
        float gain(vgain);
        if ((!_sustain || !*_sustain) && _offs[voice] >= 0 && (unsigned long) (_offs[voice]) < i + start) {
            unsigned long release = 1 + (0.01 * _sampleRate);
            unsigned long dist = i + start - _offs[voice];
            if (dist > release) {
                _ons[voice] = -1;
                break;
            }
            gain = gain * float(release - dist) / float(release);
        }
        size_t sz = int(_sizes[voice]);
        bool decay = (s > sz);
        size_t index = (s % int(sz));
        float sample = _wavetable[voice][index];
        if (decay) {
            if (index == 0) {
                sample += _wavetable[voice][sz - 1];
            }
            else {
                sample += _wavetable[voice][index - 1];
            }
            sample /= 2;
            _wavetable[voice][index] = sample;
        }
        _output[offset + i] += gain * sample;
    }
}

extern "C" {

const DSSI_Descriptor *dssi_descriptor(unsigned long index) {
    if (index == 0) {
        return &dizzYM::dssiDescriptor;
    }
    else {
        return 0;
    }
}

}
