#pragma once

#include <boost/math/common_factor_rt.hpp>
#include <cassert>
#include <cmath>

#include "util/buf.h"
#include "util/util.h"

class Config {

#ifdef DIZZYM_UNIT_TEST
public:
#endif

    static int const YM2149_ATOM_SIZE = 8;

public:

    int final _atomSize = YM2149_ATOM_SIZE;

    int const _refMidiNote = 69;

    float const _refFreq = 440;

    int const _semitones = 12;

    int final _nominalClock = 2000000; // TODO LATER: Support non-integer clock.

    double final _cutoff = .475, _transition = .05; // Both normalised to rate so in [0, .5].

    double final _rcepsAddBeforeLog = 1e-50;

    sizex final _minBLEPCount = 1000;

    float const _maxAmp = .5;

    int const _alphaCC = 80, _betaCC = 81;

    int naiveRate() const {
        assert(!((_nominalClock * _atomSize) % YM2149_ATOM_SIZE));
        return _nominalClock * _atomSize / YM2149_ATOM_SIZE;
    }

    int evenEmpiricalOrder() const {
        double const empirical = 4 / _transition; // According to Steven W. Smith.
        return int(round(empirical / 2)) * 2; // Closest even int to empirical.
    }

    int idealMinBLEPCount(int const pcmRate) const {
        int const naiveRate = this->naiveRate();
        return naiveRate / boost::math::gcd(naiveRate, pcmRate);
    }

};
