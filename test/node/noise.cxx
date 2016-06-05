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
    for (sizex kMax = NOISE.limit() - sizex(expected.size()), k = kMax - kMax; k < kMax; ++k) {
        bool match = true;
        for (auto n = expected.size(), i = n - n; i < n; ++i) {
            if (expected[i] != NOISE.at(sizex(k + i))) {
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

BOOST_AUTO_TEST_SUITE_END()
