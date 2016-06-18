#pragma once

#include <cmath>
#include <complex>

#include "../config.h"
#include "../dssi/plugin.h"
#include "buf.h"

class Paster;

class MinBLEPs {

    friend Paster;

    double const _ratio;

#ifdef CARSTAIRS_TEST

public:

    Buffer<double> _BLI {"_BLI"};

    Buffer<std::complex<double>> _realCepstrum {"_realCepstrum"};

#endif

    sizex const _minBLEPCount;

    Buffer<float> _minBLEPs {"_minBLEPs"};

public:

    MinBLEPs(Config const&, int pcmRate);

    DSSI::cursor getNaiveCursor(DSSI::cursor pcmCursor) const {
        // First naive cursor that doesn't affect pcm cursor values prior to that given:
        return DSSI::cursor(floor(double(pcmCursor - 1) / _ratio) + 1);
    }

};
