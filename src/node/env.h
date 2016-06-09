#pragma once

#include "../config.h"
#include "../state.h"
#include "osc.h"

class Env: public Osc {

public:

    Env(Config const&, State const&);

};
