#include "env.h"

#include "../util/buf.h"

namespace {

class Env1000: public Buffer<int> {

public:

    Env1000()
            : Buffer("ENV_1000", 32) {
        for (auto i = 0; i < 32; ++i) {
            put(i, 31 - i);
        }
    }

} ENV_1000;

class Env1010: public Buffer<int> {

public:

    Env1010()
            : Buffer("ENV_1010", 64) {
        for (auto i = 0; i < 32; ++i) {
            put(i, 31 - i);
            put(32 + i, i);
        }
    }

} ENV_1010;

class Env1100: public Buffer<int> {

public:

    Env1100()
            : Buffer("ENV_1100", 32) {
        for (auto i = 0; i < 32; ++i) {
            put(i, i);
        }
    }

} ENV_1100;

class Env1110: public Buffer<int> {

public:

    Env1110()
            : Buffer("ENV_1110", 64) {
        for (auto i = 0; i < 32; ++i) {
            put(i, i);
            put(32 + i, 31 - i);
        }
    }

} ENV_1110;

}

Env::Env(Config const& config, State const& state)
        : Osc(config._atomSize, state, "Envelope", ENV_1000, state.EP(), true) {
}

void Env::shapeChanged(int shape) {
    switch (shape) {
        case 8:
            setShape(ENV_1000);
            break;
        case 10:
            setShape(ENV_1010);
            break;
        case 12:
            setShape(ENV_1100);
            break;
        case 14:
            setShape(ENV_1110);
            break;
    }
}
