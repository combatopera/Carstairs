#pragma once

#include <alsa/seq_event.h>
#include <ladspa.h>
#include <stddef.h>

#include "dssi/port.h"
#include "node/pcm.h"
#include "state.h"
#include "util.h"

class Constants {

public:

    PortInfo OUTPUT_PORT_INFO;

    PortInfo SUSTAIN_PORT_INFO;

    PortInfo *PORT_INFOS[2];

    size_t const PortCount;

    Constants();

};

extern Constants CONSTANTS;

class dizzYM {

    Buffer<LADSPA_Data *> _portValPtrs;

    int _sampleRate;

    unsigned long _sampleCursor;

    State _state;

    PCM _chip;

public:

    dizzYM(int sampleRate);

    void reset();

    void setPortValPtr(int, LADSPA_Data *);

    void runSynth(unsigned long, snd_seq_event_t *, unsigned long);

};
