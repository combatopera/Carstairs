#pragma once

#include <complex>

#include "../config.h"
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

};
