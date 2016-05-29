#pragma once

#include "state.h"

class Program: public Fire {

    float const _rate;

public:

    Program(float rate)
            : _rate(rate) {
    }

    float rate() const {
        return _rate;
    }

    void fire(int, int, State&) const;

};
