#pragma once

#include <ladspa.h>

#include "../config.h"
#include "../node.h"
#include "../state.h"
#include "../util/minbleps.h"
#include "level.h"

class PCM: public Node<LADSPA_Data> {

    MinBLEPs const _minBLEPs;

    Level * const _level;

    void resetImpl();

    void renderImpl();

public:

    PCM(Config const *, State *, Level *);

};
