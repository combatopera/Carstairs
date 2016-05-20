#pragma once

#include <cmath>

class Config {

    static int const YM2149_ATOM_SIZE = 8;

public:

    int _atomSize = YM2149_ATOM_SIZE;

    int const _refMidiNote = 69;

    float const _refFreq = 440;

    int const _semitones = 12;

    int const _nominalClock = 2000000; // TODO LATER: Support non-integer clock.

    int const _pcmRate; // DSSI doesn't support non-integer rates.

    double const _cutoff = .475, _transition = .05; // Both normalised to rate so in [0, .5].

    Config(int pcmRate)
            : _pcmRate(pcmRate) {
    }

    int naiveRate() const {
        return _nominalClock * _atomSize / YM2149_ATOM_SIZE; // TODO LATER: Enforce exact division.
    }

    int evenEmpiricalOrder() const {
        double const empirical = 4 / _transition; // According to Steven W. Smith.
        return int(round(empirical / 2)) * 2; // Closest even int to empirical.
    }

};
