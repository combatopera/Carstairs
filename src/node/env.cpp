#include "env.h"

#include "../util/buf.h"

namespace {

class Env1000: public PeriodicShape {

public:

    Env1000()
            : PeriodicShape("ENV_1000", 32) {
        for (auto i = 0; i < 32; ++i) {
            _data.put(i, 31 - i);
        }
    }

} ENV_1000;

class Env1001: public HoldShape {

public:

    Env1001()
            : HoldShape("ENV_1001", 32) {
        for (auto i = 0; i < 32; ++i) {
            _data.put(i, 31 - i);
        }
    }

} ENV_1001;

class Env1010: public PeriodicShape {

public:

    Env1010()
            : PeriodicShape("ENV_1010", 64) {
        for (auto i = 0; i < 32; ++i) {
            _data.put(i, 31 - i);
            _data.put(32 + i, i);
        }
    }

} ENV_1010;

class Env1011: public HoldShape {

public:

    Env1011()
            : HoldShape("ENV_1011", 33) {
        for (auto i = 0; i < 32; ++i) {
            _data.put(i, 31 - i);
        }
        _data.put(32, 31);
    }

} ENV_1011;

class Env1100: public PeriodicShape {

public:

    Env1100()
            : PeriodicShape("ENV_1100", 32) {
        for (auto i = 0; i < 32; ++i) {
            _data.put(i, i);
        }
    }

} ENV_1100;

class Env1101: public HoldShape {

public:

    Env1101()
            : HoldShape("ENV_1101", 32) {
        for (auto i = 0; i < 32; ++i) {
            _data.put(i, i);
        }
    }

} ENV_1101;

class Env1110: public PeriodicShape {

public:

    Env1110()
            : PeriodicShape("ENV_1110", 64) {
        for (auto i = 0; i < 32; ++i) {
            _data.put(i, i);
            _data.put(32 + i, 31 - i);
        }
    }

} ENV_1110;

class Env1111: public HoldShape {

public:

    Env1111()
            : HoldShape("ENV_1111", 33) {
        for (auto i = 0; i < 32; ++i) {
            _data.put(i, i);
        }
        _data.put(32, 0);
    }

} ENV_1111;

}

Env::Env(Config const& config, State const& state)
        : Osc(config._atomSize, state, "Envelope", ENV_1000, state.EP(), true) {
}

void Env::shapeChanged(int shape) {
    switch (shape) {
        case 0:
        case 1:
        case 2:
        case 3:
            setShape(ENV_1001);
            break;
        case 4:
        case 5:
        case 6:
        case 7:
            setShape(ENV_1111);
            break;
        case 8:
            setShape(ENV_1000);
            break;
        case 9:
            setShape(ENV_1001);
            break;
        case 10:
            setShape(ENV_1010);
            break;
        case 11:
            setShape(ENV_1011);
            break;
        case 12:
            setShape(ENV_1100);
            break;
        case 13:
            setShape(ENV_1101);
            break;
        case 14:
            setShape(ENV_1110);
            break;
        case 15:
            setShape(ENV_1111);
            break;
    }
}
