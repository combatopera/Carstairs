#pragma once

class Config {

    static int const YM2149_ATOM_SIZE = 8;

public:

    int _atomSize = YM2149_ATOM_SIZE;

    int const _refMidiNote = 69;

    float const _refFreq = 440;

    int const _semitones = 12;

    float const _nominalClock = 2000000;

    float const _pcmRate;

    double const _cutoff = .475, _transition = .05;

    Config(float pcmRate)
            : _pcmRate(pcmRate) {
    }

    float pluginClock() const {
        return _nominalClock / YM2149_ATOM_SIZE * float(_atomSize);
    }

};
