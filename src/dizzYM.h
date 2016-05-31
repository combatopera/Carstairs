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

    PortInfo_t const _pcm, _alpha, _beta;

    Values<PortInfo_t const> const _values;

    PortInfoEnum(Config const&, sizex ord = 0);

};

class dizzYM {

    PortInfoEnum const& _PortInfo;

    Buffer<LADSPA_Data *> _portValPtrs;

    State _state;

    Program _loader;

    Tone _tone;

    Level _level;

    PCM _pcm;

    int const _pcmRate;

    DSSI::cursor getProgramEventX(DSSI::cursor voidX) const;

public:

    dizzYM(Config const&, PortInfoEnum const&, int pcmRate);

    void start();

    void setPortValPtr(sizex, LADSPA_Data *);

    void runSynth(DSSI::cursor, snd_seq_event_t const *, DSSI::cursor);

};
