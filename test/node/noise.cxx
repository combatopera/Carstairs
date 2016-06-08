#include "../../src/node/noise.h"

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <array>

#include "../../src/node.h"

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

BOOST_FIXTURE_TEST_CASE(carry, F) {
    State state(_config);
    state._NP = 1;
    auto size = 17 * 16 + 1;
    Noise oRef(_config, state);
    oRef.start();
    auto ref = oRef.render(size);
    for (auto n = 1; n < size; ++n) {
        Noise o(_config, state);
        o.start();
        auto v1 = o.render(n);
        BOOST_CHECK_EQUAL_COLLECTIONS(ref.begin(), ref.begin(n), v1.begin(), v1.end());
        auto v2 = o.render(size);
        BOOST_CHECK_EQUAL_COLLECTIONS(ref.begin(n), ref.end(), v2.begin(), v2.end());
    }
}

BOOST_FIXTURE_TEST_CASE(increasePeriodOnBoundary, F) {
    _config._atomSize = 4;
    State state(_config);
    state._NP = 1;
    Buffer<int> oneZeroShape("oneZeroShape", 2), ones("ones", 24), zeros("zeros", 15);
    oneZeroShape.put(0, 1);
    oneZeroShape.put(1, 0);
    ones.fill(1);
    zeros.zero();
    Noise o(_config, state, oneZeroShape);
    o.start();
    {
        auto v = o.render(16);
        BOOST_CHECK_EQUAL_COLLECTIONS(ones.begin(), ones.begin(8), v.begin(), v.begin(8));
        BOOST_CHECK_EQUAL_COLLECTIONS(zeros.begin(), zeros.begin(8), v.begin(8), v.end());
    }
    state._NP = 2;
    {
        auto v = o.render(o.cursor() + 31);
        BOOST_CHECK_EQUAL_COLLECTIONS(ones.begin(), ones.begin(16), v.begin(), v.begin(16));
        BOOST_CHECK_EQUAL_COLLECTIONS(zeros.begin(), zeros.begin(15), v.begin(16), v.end());
    }
    state._NP = 3;
    {
        auto v = o.render(o.cursor() + 26);
        BOOST_CHECK_EQUAL_COLLECTIONS(zeros.begin(), zeros.begin(1), v.begin(), v.begin(1));
        BOOST_CHECK_EQUAL_COLLECTIONS(ones.begin(), ones.begin(24), v.begin(1), v.begin(25));
        BOOST_CHECK_EQUAL_COLLECTIONS(zeros.begin(), zeros.begin(1), v.begin(25), v.end());
    }
}

BOOST_FIXTURE_TEST_CASE(decreasePeriodOnBoundary, F) {
    _config._atomSize = 4;
    State state(_config);
    state._NP = 3;
    Buffer<int> oneZeroShape("oneZeroShape", 2), ones("ones", 24), zeros("zeros", 24);
    oneZeroShape.put(0, 1);
    oneZeroShape.put(1, 0);
    ones.fill(1);
    zeros.zero();
    Noise o(_config, state, oneZeroShape);
    o.start();
    {
        auto v = o.render(48);
        BOOST_CHECK_EQUAL_COLLECTIONS(ones.begin(), ones.begin(24), v.begin(), v.begin(24));
        BOOST_CHECK_EQUAL_COLLECTIONS(zeros.begin(), zeros.begin(24), v.begin(24), v.end());
    }
    state._NP = 2;
    {
        auto v = o.render(o.cursor() + 38);
        BOOST_CHECK_EQUAL_COLLECTIONS(ones.begin(), ones.begin(16), v.begin(), v.begin(16));
        BOOST_CHECK_EQUAL_COLLECTIONS(zeros.begin(), zeros.begin(16), v.begin(16), v.begin(32));
        BOOST_CHECK_EQUAL_COLLECTIONS(ones.begin(), ones.begin(6), v.begin(32), v.end());
    }
    state._NP = 1;
    {
        auto v = o.render(o.cursor() + 27);
        BOOST_CHECK_EQUAL_COLLECTIONS(ones.begin(), ones.begin(10), v.begin(), v.begin(10));
        BOOST_CHECK_EQUAL_COLLECTIONS(zeros.begin(), zeros.begin(8), v.begin(10), v.begin(18));
        BOOST_CHECK_EQUAL_COLLECTIONS(ones.begin(), ones.begin(8), v.begin(18), v.begin(26));
        BOOST_CHECK_EQUAL_COLLECTIONS(zeros.begin(), zeros.begin(1), v.begin(26), v.end());
    }
}

BOOST_FIXTURE_TEST_CASE(stepBiggerThanBlock, F) {
    _config._atomSize = 1;
    State state(_config);
    state._NP = 5;
    Buffer<int> oneZeroShape("oneZeroShape", 2), ones("ones", 24), zeros("zeros", 24);
    oneZeroShape.put(0, 1);
    oneZeroShape.put(1, 0);
    ones.fill(1);
    zeros.zero();
    Noise o(_config, state, oneZeroShape);
    o.start();
    {
        auto v = o.render(3);
        BOOST_CHECK_EQUAL_COLLECTIONS(ones.begin(), ones.begin(3), v.begin(), v.end());
        BOOST_CHECK_EQUAL(10, o._stepSize);
        BOOST_CHECK_EQUAL(3, o._progress);
    }
    state._NP = 6;
    {
        auto v = o.render(o.cursor() + 4);
        BOOST_CHECK_EQUAL_COLLECTIONS(ones.begin(), ones.begin(4), v.begin(), v.end());
        BOOST_CHECK_EQUAL(10, o._stepSize);
        BOOST_CHECK_EQUAL(7, o._progress);
    }
    {
        auto v = o.render(o.cursor() + 16);
        BOOST_CHECK_EQUAL_COLLECTIONS(ones.begin(), ones.begin(3), v.begin(), v.begin(3));
        BOOST_CHECK_EQUAL_COLLECTIONS(zeros.begin(), zeros.begin(12), v.begin(3), v.begin(15));
        BOOST_CHECK_EQUAL_COLLECTIONS(ones.begin(), ones.begin(1), v.begin(15), v.end());
        BOOST_CHECK_EQUAL(12, o._stepSize);
        BOOST_CHECK_EQUAL(1, o._progress);
    }
}

BOOST_AUTO_TEST_SUITE_END()
