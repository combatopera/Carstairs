#include <array>

#include "../util/buf.h"

#ifndef CARSTAIRS_TEST
namespace {
#endif

class Noise: public Buffer<int> {

    static int step(int const mask, int& x) {
        auto const bit = x & 1;
        x >>= 1;
        if (bit) {
            x ^= mask;
        }
        return 1 - bit; // Authentic, see qnoispec.
    }

public:

    Noise()
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

} NOISE;

#ifndef CARSTAIRS_TEST
}
#endif
