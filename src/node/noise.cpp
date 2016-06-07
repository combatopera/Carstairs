#include "noise.h"

#include <array>

namespace {

class YM2149Noise: public Buffer<int> {

    static int step(int const mask, int& x) {
        auto const bit = x & 1;
        x >>= 1;
        if (bit) {
            x ^= mask;
        }
        return 1 - bit; // Authentic, see qnoispec.
    }

public:

    YM2149Noise()
            : Buffer("NOISE") {
        std::array<int, 2> constexpr YM2149_POSITIVE_DEGREES {17, 14};
        auto mask = 0;
        for (auto const positiveDegree : YM2149_POSITIVE_DEGREES) {
            mask += 1 << (positiveDegree - 1);
        }
        auto const initialX = 1;
        auto n = 0;
        {
            auto x = initialX;
            do {
                step(mask, x);
                ++n;
            } while (initialX != x);
        }
        setLimit(n);
        for (auto x = initialX, i = 0; i < n; ++i) {
            put(i, step(mask, x));
        }
    }

} YM2149_NOISE;

}

Noise::Noise(Config const& config, State const& state)
        : Noise(config, state, YM2149_NOISE) {
}

Noise::Noise(Config const& config, State const& state, View<int> const shape)
        : Node("Noise", state), _atomSize(config._atomSize), _shape(shape), _indexInShape(), _progress(), _stepSize() {
}

#ifdef CARSTAIRS_TEST
Buffer<int> const& noiseShape() {
    return YM2149_NOISE;
}
#endif
