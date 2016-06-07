#pragma once

#include "../config.h"
#include "../state.h"
#include "osc.h"

class Tone: public Osc {

public:

    Tone(Config const&, State const&);

};
