#pragma once

#include <alsa/seq_event.h>
#include <ladspa.h>

#include "dssi/port.h"
#include "node/pcm.h"
#include "state.h"
#include "util.h"

class PortInfoEnum {

public:

    PortInfo_t _output, _sustain;

    Values<PortInfo_t> _values;

    PortInfoEnum(index_t ord = 0);

};

extern PortInfoEnum PortInfo;

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
