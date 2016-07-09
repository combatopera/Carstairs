// Copyright 2016 Andrzej Cichocki

// This file is part of Carstairs.
//
// Carstairs is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Carstairs is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Carstairs.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <boost/filesystem/path.hpp>
#include <boost/math/common_factor_rt.hpp>
#include <cassert>
#include <cmath>

#include "py/interpreter.h"
#include "py/main.h"
#include "py/py.h"
#include "util/buf.h"
#include "util/util.h"

class Config {

    static Log const LOG;

public:

    sizex const YM2149_ATOM_SIZE = 8; // XXX: Why symbol not found when static?

    unsigned long _UniqueID;

    sizex CARSTAIRS_CONST _atomSize = 1;

    int const _refMidiNote = 69;

    float const _refFreq = 440;

    int const _semitones = 12;

    float CARSTAIRS_CONST _nominalClock = 2000000;

    double CARSTAIRS_CONST _transition = .1, _stopband = 0; // Both normalised to rate so in [0, .5].

    double CARSTAIRS_CONST _rcepsAddBeforeLog = 1e-50;

    sizex CARSTAIRS_CONST _minBLEPCount = 1000; // Empirically enough to make artifacts invisible.

    float const _maxAmp = .5;

    int const _alphaCC = 80, _betaCC = 81;

    boost::filesystem::path _modulesDir;

#ifdef CARSTAIRS_TEST
    Config()
            : _UniqueID() {
    }
#endif

    Config(Python const& python) {
        Interpreter(python, [] {}).runTask(
                [&] {
                    Interpreter::execute(
#include "config.raw"
);
                    auto const config = Interpreter::import("__main__");
                    auto attr = config.getAttr("uniqueid");
                    if (attr) {
                        _UniqueID = attr.numberRoundToUnsignedLong(); // TODO LATER: Should not round.
                    }
                    CARSTAIRS_DEBUG("UniqueID: %lu", _UniqueID);
                    attr = config.getAttr("modulesdir");
                    if (attr) {
                        _modulesDir = attr.toPathBytes().unwrapBytes();
                    }
                    else {
                        _modulesDir = config.getAttr("userhome").toPathBytes().unwrapBytes();
                        _modulesDir /= ".carstairs";
                    }
                });
    }

    float naiveRate() const {
        return _nominalClock * float(_atomSize) / float(YM2149_ATOM_SIZE);
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
