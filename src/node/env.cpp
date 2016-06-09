#include "env.h"

#include "../util/buf.h"

namespace {

class Env1000: public Buffer<int> {

public:

    Env1000()
            : Buffer("Env1000", 32) {
        for (auto i = 0; i < 32; ++i) {
            put(i, 31 - i);
        }
    }

} ENV_1000;

}

Env::Env(Config const& config, State const& state)
        : Osc(config._atomSize, state, "Envelope", ENV_1000, state.EP(), true) {
}
