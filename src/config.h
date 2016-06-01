#pragma once

#include <boost/math/common_factor_rt.hpp>
#include <cassert>
#include <cmath>

#include "util/buf.h"
#include "util/util.h"

class Config {

#ifdef CARSTAIRS_TEST
public:
#endif

    static int const YM2149_ATOM_SIZE = 8;

public:

    unsigned long const _UniqueID = 0;

    int FINAL _atomSize = 1;

    int const _refMidiNote = 69;

    float const _refFreq = 440;

    int const _semitones = 12;

    float FINAL _nominalClock = 2000000;

    double FINAL _transition = .1, _stopband = 0; // Both normalised to rate so in [0, .5].

    double FINAL _rcepsAddBeforeLog = 1e-50;

    sizex FINAL _minBLEPCount = 1000; // Empirically enough to make artifacts invisible.

    float const _maxAmp = .5;

    int const _alphaCC = 80, _betaCC = 81;

    char const * const _programModule = "program";

    float naiveRate() const {
        return _nominalClock * float(_atomSize) / YM2149_ATOM_SIZE;
    }

    int evenEmpiricalOrder() const {
        double const empirical = 4 / _transition; // According to Steven W. Smith.
        return int(round(empirical / 2)) * 2; // Closest even int to empirical.
    }

    double cutoff() const {
        return .5 - _stopband - _transition / 2;
    }

    int idealMinBLEPCount(int const pcmRate) const {
        float const naiveRate = this->naiveRate();
        assert(roundf(naiveRate) == naiveRate);
        return int(naiveRate) / boost::math::gcd(int(naiveRate), pcmRate);
    }

};
