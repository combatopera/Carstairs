#pragma once

#include <alsa/seq_event.h>
#include <dssi.h>
#include <ladspa.h>
#include <stddef.h>

#include "dssi/port.h"
#include "node/pcm.h"
#include "state.h"

static PortInfo OUTPUT_PORT_INFO(0, true, true, "Output", 0, 0, 0, DSSI_NONE);

static PortInfo SUSTAIN_PORT_INFO(1, false, false, "Sustain (on/off)",
LADSPA_HINT_DEFAULT_MINIMUM | LADSPA_HINT_INTEGER | LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE, 0, 1, DSSI_CC(64));

static PortInfo *PORT_INFOS[] = {&OUTPUT_PORT_INFO, &SUSTAIN_PORT_INFO};

static size_t const PortCount = (sizeof PORT_INFOS) / (sizeof PORT_INFOS[0]);

class dizzYM {

    friend class Descriptor;

    static void connect_port(LADSPA_Handle, unsigned long, LADSPA_Data *);

    static void activate(LADSPA_Handle);

    static int get_midi_controller_for_port(LADSPA_Handle, unsigned long);

    static void run(LADSPA_Handle, unsigned long);

    static void run_synth(LADSPA_Handle, unsigned long, snd_seq_event_t *, unsigned long);

    void runSynth(unsigned long, snd_seq_event_t *, unsigned long);

    LADSPA_Data *_portValPtrs[PortCount];

    int _sampleRate;

    unsigned long _sampleCursor;

    State _state;

    PCM _chip;

public:

    dizzYM(int sampleRate);

};
