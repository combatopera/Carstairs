#include "../util/buf.h"

namespace {

class Noise: public Buffer<int> {

public:

    Noise()
            : Buffer("NOISE") {
        auto mask = 0;
        mask += 1 << (17 - 1);
        mask += 1 << (14 - 1);
        auto n = 0;
        {
            auto x = 1;
            do {
                auto bit = x & 1;
                x >>= 1;
                if (bit) {
                    x ^= mask;
                }
                ++n;
            } while (1 != x);
        }
        setLimit(n);
        auto i = 0;
        {
            auto x = 1;
            do {
                auto bit = x & 1;
                x >>= 1;
                if (bit) {
                    x ^= mask;
                }
                put(i++, 1 - bit); // Authentic, see qnoispec.
            } while (1 != x);
        }
    }

} NOISE;

}
