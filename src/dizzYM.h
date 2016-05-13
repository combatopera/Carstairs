#pragma once

#include <alsa/seq_event.h>
#include <ladspa.h>

#include "buf.h"
#include "config.h"
#include "dssi/port.h"
#include "node/pcm.h"
#include "node/tone.h"
#include "state.h"
#include "util/enum.h"
#include "util/util.h"

class PortInfoEnum {

public:

    PortInfo_t _output, _sustain;

    Values<PortInfo_t> _values;

    PortInfoEnum(index_t ord = 0);

};

extern PortInfoEnum PortInfo;

class dizzYM {

    static cursor_t const INITIAL_SAMPLE_CURSOR = 0;

    Buffer<LADSPA_Data *> _portValPtrs;

    int _sampleRate;

    cursor_t _sampleCursor;

    Config const _config;

    State _state;

    Tone _tone;

    PCM _chip;

public:

    dizzYM(int sampleRate);

    void reset();

    void setPortValPtr(int, LADSPA_Data *);

    void runSynth(cursor_t, snd_seq_event_t *, cursor_t);

};
