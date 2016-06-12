#pragma once

#include "../config.h"
#include "../state.h"
#include "osc.h"

class Env: public Osc, public EnvShape {

    void shapeChanged(int);

public:

    Env(Config const&, State const&);

};
