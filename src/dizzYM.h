#include <alsa/seq_event.h>
#include <ladspa.h>
#include <array>

#include "port.h"

int const MIDI_NOTE_COUNT = 128;

const std::array<PortInfo, 2> PORT_INFOS = { //
        PortInfo(LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO, "Output", 0, 0, 0), //
        PortInfo(LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL, "Sustain (on/off)",
        LADSPA_HINT_DEFAULT_MINIMUM | LADSPA_HINT_INTEGER | LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE, 0, 1), //
        };

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

    LADSPA_Data *_output, *_sustain;

    int _sampleRate;

    long _sampleCursor;

    long _ons[MIDI_NOTE_COUNT];

    long _offs[MIDI_NOTE_COUNT];

    int _velocities[MIDI_NOTE_COUNT];

    float *_wavetable[MIDI_NOTE_COUNT];

    float _sizes[MIDI_NOTE_COUNT];

};
