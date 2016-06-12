#pragma once

#include <cstdio>

#include "../config.h"
#include "../state.h"
#include "osc.h"

class Env: public Osc, public EnvShape {

    void shapeChanged() {
        printf("shapeChanged\n");
    }

public:

    Env(Config const&, State const&);

};
