#pragma once

#include <alsa/seq_event.h>
#include <ladspa.h>

#include "config.h"
#include "dssi/plugin.h"
#include "dssi/port.h"
#include "node/level.h"
#include "node/pcm.h"
#include "node/tone.h"
#include "program.h"
#include "state.h"
#include "util/buf.h"
#include "util/enum.h"

class PortInfoEnum {

public:

    PortInfo_t _output, _alpha, _beta;

    Values<PortInfo_t> _values;

    PortInfoEnum(Config const&, sizex ord = 0);

};

extern PortInfoEnum PortInfo;

class dizzYM {

    static DSSI::cursor const INITIAL_SAMPLE_CURSOR = 0;

    Buffer<LADSPA_Data *> _portValPtrs;

    DSSI::cursor _sampleCursor;

    State _state;

    Program _program;

    Tone _tone;

    Level _level;

    PCM _chip;

public:

    dizzYM(Config const&, int pcmRate);

    void reset();

    void setPortValPtr(int, LADSPA_Data *);

    void runSynth(DSSI::cursor, snd_seq_event_t *, DSSI::cursor);

};
