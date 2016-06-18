#include "tone.h"

#include "../util/buf.h"

namespace {

class Square: public Shape {

public:

    Square()
            : Shape("SQUARE", 2) {
        _data.put(0, 1);
        _data.put(1, 0);
    }

} SQUARE;

}

Tone::Tone(Config const& config, State const& state)
        : Osc(config._atomSize, state, "Tone", SQUARE, state.TP(), true) {
}
