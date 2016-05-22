#pragma once

#include <boost/math/common_factor_rt.hpp>
#include <cassert>
#include <cmath>

#include "util/util.h"

class Config {

#ifdef DIZZYM_UNIT_TEST
public:
#endif

    static int const YM2149_ATOM_SIZE = 8;

public:

    int prodconst _atomSize = YM2149_ATOM_SIZE;

    int const _refMidiNote = 69;

    float const _refFreq = 440;

    int const _semitones = 12;

    int prodconst _nominalClock = 2000000; // TODO LATER: Support non-integer clock.

    int const _pcmRate; // DSSI doesn't support non-integer rates.

    double prodconst _cutoff = .475, _transition = .05; // Both normalised to rate so in [0, .5].

    double prodconst _rcepsAddBeforeLog = 1e-50;

    int prodconst _minBLEPCount = 1000;

    Config(int pcmRate)
            : _pcmRate(pcmRate) {
    }

    int naiveRate() const {
        assert(!((_nominalClock * _atomSize) % YM2149_ATOM_SIZE));
        return _nominalClock * _atomSize / YM2149_ATOM_SIZE;
    }

    int evenEmpiricalOrder() const {
        double const empirical = 4 / _transition; // According to Steven W. Smith.
        return int(round(empirical / 2)) * 2; // Closest even int to empirical.
    }

    int idealMinBLEPCount() const {
        int naiveRate = this->naiveRate();
        return naiveRate / boost::math::gcd(naiveRate, _pcmRate);
    }

};
