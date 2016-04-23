#include <alsa/seq_event.h>
#include <ladspa.h>
#include <array>

#include "port.h"

int const MIDI_NOTE_COUNT = 128;

class dizzYM {

    friend class Descriptor;

    dizzYM(int sampleRate);

    ~dizzYM();

    static LADSPA_Handle instantiate(const LADSPA_Descriptor *, unsigned long);

    static void cleanup(LADSPA_Handle);

    static void connect_port(LADSPA_Handle, unsigned long, LADSPA_Data *);

    static void activate(LADSPA_Handle);

    static int get_midi_controller_for_port(LADSPA_Handle, unsigned long);

    static void run(LADSPA_Handle, unsigned long);

    static void run_synth(LADSPA_Handle, unsigned long, snd_seq_event_t *, unsigned long);

    void runSynth(unsigned long, snd_seq_event_t *, unsigned long);

    void addSamples(int, unsigned long, unsigned long);

    float *_output;

    float *_sustain;

    int _sampleRate;

    long _sampleCursor;

    long _ons[MIDI_NOTE_COUNT];

    long _offs[MIDI_NOTE_COUNT];

    int _velocities[MIDI_NOTE_COUNT];

    float *_wavetable[MIDI_NOTE_COUNT];

    float _sizes[MIDI_NOTE_COUNT];

public:

    static const std::array<Port, 2> PORTS;

};
