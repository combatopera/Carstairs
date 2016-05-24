#pragma once

#include <ladspa.h>

#include "../config.h"
#include "../node.h"
#include "../state.h"
#include "../util/buf.h"
#include "../util/minbleps.h"

class PCM: public Node<LADSPA_Data> {

    constexpr static float INITIAL_DC = 0;

    MinBLEPs _minBLEPs;

    Node<float>& _naive;

    Buffer<float> _derivative {"_derivative"}, _pcm {"_pcm"};

    float _dc;

    sizex _overflowIndex;

    void resetImpl();

    void renderImpl();

public:

    PCM(Config const&, State&, Node<float>&);

};
