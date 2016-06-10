#pragma once

#include <alsa/seq_event.h>
#include <ladspa.h>
#include <memory>

#include "config.h"
#include "dssi/plugin.h"
#include "dssi/port.h"
#include "module.h"
#include "node/env.h"
#include "node/level.h"
#include "node/mixer.h"
#include "node/noise.h"
#include "node/pcm.h"
#include "node/tone.h"
#include "program.h"
#include "py/main.h"
#include "state.h"
#include "util/buf.h"
#include "util/enum.h"

class PortInfoEnum {

public:

    PortInfo_t const PCM, ALPHA, BETA;

private:

    Values<PortInfo_t const> const _values;

public:

    PortInfoEnum(Config const&, sizex ord = 0);

    Values<PortInfo_t const> const& values() const {
        return _values;
    }

};

class Carstairs {

    PortInfoEnum const& _PortInfo;

    Buffer<LADSPA_Data *> _portValPtrs;

    State _state;

    Loader _loader;

    Tone _tone;

    Noise _noise;

    Mixer _mixer;

    Env _env;

    Level _level;

    PCM _pcm;

    int const _pcmRate;

    sizex _pendingProgram = 0; // XXX: Or the DefaultProgram?

    std::shared_ptr<Program const> _currentProgram;

    DSSI::cursor getProgramEventX(DSSI::cursor voidX) const;

public:

    Carstairs(Config const&, PortInfoEnum const&, Module const&, Python const&, ProgramInfos const&, int pcmRate);

    void start();

    void setPortValPtr(sizex, LADSPA_Data *);

    void runSynth(DSSI::cursor, snd_seq_event_t const *, DSSI::cursor);

    void selectProgram(sizex index) {
        _pendingProgram = index;
    }

};
