#pragma once

#include <boost/filesystem/path.hpp>
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

    int CARSTAIRS_CONST _atomSize = 1;

    int const _refMidiNote = 69;

    float const _refFreq = 440;

    int const _semitones = 12;

    float CARSTAIRS_CONST _nominalClock = 2000000;

    double CARSTAIRS_CONST _transition = .1, _stopband = 0; // Both normalised to rate so in [0, .5].

    double CARSTAIRS_CONST _rcepsAddBeforeLog = 1e-50;

    sizex CARSTAIRS_CONST _minBLEPCount = 1000; // Empirically enough to make artifacts invisible.

    float const _maxAmp = .5;

    int const _alphaCC = 80, _betaCC = 81;

    boost::filesystem::path const _modulesDir = "/home/arc/projects/sounds/carstairs";

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
