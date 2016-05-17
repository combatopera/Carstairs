#pragma once

#include <ladspa.h>

#include "../config.h"
#include "../node.h"
#include "../state.h"
#include "../util/minbleps.h"

class PCM: public Node<LADSPA_Data> {

    MinBLEPs const _minBLEPs;

    Node<float> * const _naive;

    void resetImpl();

    void renderImpl();

public:

    PCM(Config const *, State *, Node<float> *);

};
