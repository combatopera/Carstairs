#pragma once

#include "../config.h"
#include "../state.h"
#include "../util/buf.h"
#include "osc.h"

#ifdef CARSTAIRS_TEST
Buffer<int> const& noiseShape();
#endif

class Noise: public Osc {

#ifdef CARSTAIRS_TEST
public:
#endif

    Noise(Config const&, State const&, Shape const&);

public:

    Noise(Config const&, State const&);

};
