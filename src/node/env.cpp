#include "env.h"

#include "../util/buf.h"

namespace {

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

}

Env::Env(Config const& config, State const& state)
        : Osc(config._atomSize, state, "Envelope", ENV_1000, state.EP(), true) { // FIXME: Correct initial shape.
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
