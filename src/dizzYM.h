#include <alsa/seq_event.h>
#include <dssi.h>
#include <ladspa.h>
#include <array>

#include "port.h"

class dizzYM {

    dizzYM(int sampleRate);

    ~dizzYM();

public:

    enum {

        OutputPort = 0, Sustain = 1, PortCount = 2

    };

private:

    enum {

        Notes = 128

    };

    static const LADSPA_Descriptor ladspaDescriptor;

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

    long _ons[Notes];

    long _offs[Notes];

    int _velocities[Notes];

    float *_wavetable[Notes];

    float _sizes[Notes];

public:

    static const std::array<Port, PortCount> PORTS;

    static const DSSI_Descriptor dssiDescriptor;

};
