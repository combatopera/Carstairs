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

#include "env.h"

#include <cassert>

#include "../util/buf.h"
#include "../util/util.h"

namespace {

Log const LOG(__FILE__);

class Env1000: public Shape {

public:

    Env1000()
        : Shape("ENV_1000", 32) {
        for (auto i = 0; i < 32; ++i) {
            _data.put(i, 31 - i);
        }
    }

} ENV_1000;

class Env1001: public Shape {

public:

    Env1001()
        : Shape("ENV_1001", 32, 31) {
        for (auto i = 0; i < 32; ++i) {
            _data.put(i, 31 - i);
        }
    }

} ENV_1001;

class Env1010: public Shape {

public:

    Env1010()
        : Shape("ENV_1010", 64) {
        for (auto i = 0; i < 32; ++i) {
            _data.put(i, 31 - i);
            _data.put(32 + i, i);
        }
    }

} ENV_1010;

class Env1011: public Shape {

public:

    Env1011()
        : Shape("ENV_1011", 33, 32) {
        for (auto i = 0; i < 32; ++i) {
            _data.put(i, 31 - i);
        }
        _data.put(32, 31);
    }

} ENV_1011;

class Env1100: public Shape {

public:

    Env1100()
        : Shape("ENV_1100", 32) {
        for (auto i = 0; i < 32; ++i) {
            _data.put(i, i);
        }
    }

} ENV_1100;

class Env1101: public Shape {

public:

    Env1101()
        : Shape("ENV_1101", 32, 31) {
        for (auto i = 0; i < 32; ++i) {
            _data.put(i, i);
        }
    }

} ENV_1101;

class Env1110: public Shape {

public:

    Env1110()
        : Shape("ENV_1110", 64) {
        for (auto i = 0; i < 32; ++i) {
            _data.put(i, i);
            _data.put(32 + i, 31 - i);
        }
    }

} ENV_1110;

class Env1111: public Shape {

public:

    Env1111()
        : Shape("ENV_1111", 33, 32) {
        for (auto i = 0; i < 32; ++i) {
            _data.put(i, i);
        }
        _data.put(32, 0);
    }

} ENV_1111;

Shape const& indexToShape(int index) {
    switch (index) {
    case 8:
        return ENV_1000;
    case 0:
    case 1:
    case 2:
    case 3:
    case 9:
        return ENV_1001;
    case 10:
        return ENV_1010;
    case 11:
        return ENV_1011;
    case 12:
        return ENV_1100;
    case 13:
        return ENV_1101;
    case 14:
        return ENV_1110;
    case 4:
    case 5:
    case 6:
    case 7:
    case 15:
        return ENV_1111;
    }
    CARSTAIRS_ERROR("Bad shape index: %d", index);
    assert(false);
    return ENV_1000; // Unreachable.
}

}

Env::Env(Config const& config, State const& state)
    : Osc(config._atomSize, state, "Envelope", indexToShape(shapeIndex()), state.EP(), true) {
}

void Env::shapeChanged() {
    setShape(indexToShape(shapeIndex()));
}
