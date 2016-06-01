#pragma once

#include <cmath>
#include <complex>

#include "../config.h"
#include "../dssi/plugin.h"
#include "buf.h"

class Paster;

class MinBLEPs {

    friend Paster;

    int const _pcmRate;

    float const _naiveRate;

#ifdef CARSTAIRS_TEST

public:

    Buffer<double> _BLI {"_BLI"};

    Buffer<std::complex<double>> _realCepstrum {"_realCepstrum"};

#endif

    sizex const _minBLEPCount;

    Buffer<float> _minBLEPs {"_minBLEPs"};

public:

    MinBLEPs(Config const&, int pcmRate);

    DSSI::cursor pcmXToNaiveX(DSSI::cursor pcmX) const {
        // First naiveX that doesn't affect pcmX prior to that given:
        return DSSI::cursor(floor(double(pcmX - 1) / _pcmRate * _naiveRate) + 1);
    }

};
