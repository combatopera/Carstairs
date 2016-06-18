#pragma once

#include "../config.h"
#include "../state.h"
#include "osc.h"

#ifdef CARSTAIRS_TEST
PeriodicShape const& noiseShape();
#endif

class Noise: public Osc {

#ifdef CARSTAIRS_TEST
public:
#endif

    Noise(Config const&, State const&, Shape const&);

public:

    Noise(Config const&, State const&);

};
