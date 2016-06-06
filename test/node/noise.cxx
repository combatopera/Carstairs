#include "../../src/node/noise.h"

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <array>

BOOST_AUTO_TEST_SUITE(TestNoise)

BOOST_AUTO_TEST_CASE(LFSR) {
    // Subsequence of the real LFSR from Hatari mailing list:
    std::array<int, 51> expected { //
    0, 1, 0, 0, 1, 1, 0, 1, 0, 1, //
            1, 1, 0, 0, 1, 0, 1, 0, 0, 1, //
            0, 0, 1, 1, 0, 1, 1, 1, 0, 0, //
            1, 1, 0, 0, 0, 0, 0, 1, 0, 0, //
            0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0};
    for (auto n = expected.size(), i = n - n; i < n; ++i) {
        expected[i] = 1 - expected[i]; // According to qnoispec, raw LFSR 1 maps to amp 0, so we flip our LFSR.
    }
    BOOST_REQUIRE_EQUAL((1 << 17) - 1, noiseShape().limit());
    for (sizex kMax = noiseShape().limit() - sizex(expected.size()), k = kMax - kMax; k < kMax; ++k) {
        bool match = true;
        for (auto n = expected.size(), i = n - n; i < n; ++i) {
            if (expected[i] != noiseShape().at(sizex(k + i))) {
                match = false;
                break;
            }
        }
        if (match) {
            BOOST_CHECK_EQUAL(10221, k);
        }
        else {
            BOOST_CHECK_NE(10221, k);
        }
    }
}

struct F {

    Config _config;

    F() {
        _config._atomSize = 8;
    }

};

BOOST_FIXTURE_TEST_CASE(works, F) {
    State state(_config);
    state._NP = 3;
    Noise o(_config, state);
    o.start();
    auto n = 100, x = 0;
    Buffer<int> expected("expected", 48);
    for (auto _ = 0; _ < 2; ++_) {
        auto v = o.render(o.cursor() + 48 * n);
        for (auto i = 0; i < n; ++i) {
            expected.fill(noiseShape().at(x++));
            BOOST_CHECK_EQUAL_COLLECTIONS(expected.begin(), expected.end(), v.begin(i * 48), v.begin((i + 1) * 48));
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
