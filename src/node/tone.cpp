#include "tone.h"

#include "../util/buf.h"

namespace {

class Square: public Buffer<int> {

public:

    Square()
            : Buffer("SQUARE", 2) {
        put(0, 1);
        put(1, 0);
    }

} SQUARE;

}

Tone::Tone(Config const& config, State const& state)
        : Osc(config, state, "Tone", SQUARE) {
}
