#include "tone.h"

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
        : Node("Tone", state), _atomSize(config._atomSize), _shape(SQUARE), _indexInShape(), _progress(), _stepSize() {
}
