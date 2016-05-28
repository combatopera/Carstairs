#pragma once

#include <ladspa.h>

#include "../config.h"
#include "../node.h"
#include "../state.h"
#include "../util/buf.h"
#include "../util/minbleps.h"
#include "../util/paster.h"

class PCM: public Node<LADSPA_Data> {

    MinBLEPs const _minBLEPs;

    Paster _paster;

    Node<float>& _naive;

    Buffer<float> _derivative {"_derivative"}, _pcm {"_pcm"};

    float _dc;

    sizex _overflowIndex;

    void startImpl();

    void renderImpl();

public:

    PCM(Config const&, State&, Node<float>&, int pcmRate);

};
