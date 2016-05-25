#include "../../src/node/tone.h"

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>

BOOST_AUTO_TEST_SUITE(TestTone)

struct F {

    Config _config;

    F() {
        _config._atomSize = 8;
    }

};

#define BUF(n, value, name) Buffer<int> name("name", n); \
    name.fill(value);

BOOST_FIXTURE_TEST_CASE(works, F) {
    State state(_config);
    state._TP = 3;
    Tone o(_config, state);
    BUF(24, 1, ones)
    BUF(24, 0, zeros)
    View<int> v = o.render(96);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(ones.begin(), ones.end(), v.begin(), v.begin(24));
    BOOST_REQUIRE_EQUAL_COLLECTIONS(zeros.begin(), zeros.end(), v.begin(24), v.begin(48));
    BOOST_REQUIRE_EQUAL_COLLECTIONS(ones.begin(), ones.end(), v.begin(48), v.begin(72));
    BOOST_REQUIRE_EQUAL_COLLECTIONS(zeros.begin(), zeros.end(), v.begin(72), v.end());
    v = o.render(v.limit() + 48);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(ones.begin(), ones.end(), v.begin(), v.begin(24));
    BOOST_REQUIRE_EQUAL_COLLECTIONS(zeros.begin(), zeros.end(), v.begin(24), v.end());
}

BOOST_FIXTURE_TEST_CASE(resume, F) {
    State state(_config);
    state._TP = 3;
    Tone o(_config, state);
    BUF(24, 1, ones)
    BUF(24, 0, zeros)
    View<int> v = o.render(25);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(ones.begin(), ones.end(), v.begin(), v.begin(24));
    BOOST_REQUIRE_EQUAL_COLLECTIONS(zeros.begin(), zeros.begin(1), v.begin(24), v.end());
    v = o.render(v.limit() + 24);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(zeros.begin(), zeros.begin(23), v.begin(), v.begin(23));
    BOOST_REQUIRE_EQUAL_COLLECTIONS(ones.begin(), ones.begin(1), v.begin(23), v.end());
}

BOOST_FIXTURE_TEST_CASE(carry, F) {
    State state(_config);
    state._TP = 1;
    int size = 3 * 8 + 1;
    Tone refOsc(_config, state); // Must stay in scope for ref to be valid.
    View<int> ref = refOsc.render(size);
    for (int n = 0; n <= size; ++n) {
        Tone o(_config, state);
        View<int> v = o.render(n);
        BOOST_REQUIRE_EQUAL_COLLECTIONS(ref.begin(), ref.begin(v.limit()), v.begin(), v.end());
        v = o.render(size);
        BOOST_REQUIRE_EQUAL_COLLECTIONS(ref.end(v.limit()), ref.end(), v.begin(), v.end());
    }
}

BOOST_FIXTURE_TEST_CASE(endExistingStepAtEndOfBlock, F) {
    State state(_config);
    state._TP = 1;
    Tone o(_config, state);
    BUF(4, 1, ones)
    BUF(4, 0, zeros)
    View<int> v = o.render(4);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(ones.begin(), ones.end(), v.begin(), v.end());
    v = o.render(8);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(ones.begin(), ones.end(), v.begin(), v.end());
    v = o.render(12);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(zeros.begin(), zeros.end(), v.begin(), v.end());
}

BOOST_FIXTURE_TEST_CASE(increasePeriodOnBoundary, F) {
    State state(_config);
    state._TP = 1;
    Tone o(_config, state);
    BUF(24, 1, ones)
    BUF(24, 0, zeros)
    View<int> v = o.render(16);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(ones.begin(), ones.begin(8), v.begin(), v.begin(8));
    BOOST_REQUIRE_EQUAL_COLLECTIONS(zeros.begin(), zeros.begin(8), v.begin(8), v.end());
    state._TP = 2;
    v = o.render(o.cursor() + 31);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(ones.begin(), ones.begin(16), v.begin(), v.begin(16));
    BOOST_REQUIRE_EQUAL_COLLECTIONS(zeros.begin(), zeros.begin(15), v.begin(16), v.end());
    state._TP = 3;
    v = o.render(o.cursor() + 34);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(zeros.begin(), zeros.begin(9), v.begin(), v.begin(9));
    BOOST_REQUIRE_EQUAL_COLLECTIONS(ones.begin(), ones.begin(24), v.begin(9), v.begin(33));
    BOOST_REQUIRE_EQUAL_COLLECTIONS(zeros.begin(), zeros.begin(1), v.begin(33), v.end());
}

BOOST_FIXTURE_TEST_CASE(decreasePeriodOnBoundary, F) {
    State state(_config);
    state._TP = 3;
    Tone o(_config, state);
    BUF(24, 1, ones)
    BUF(24, 0, zeros)
    View<int> v = o.render(48);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(ones.begin(), ones.begin(24), v.begin(), v.begin(24));
    BOOST_REQUIRE_EQUAL_COLLECTIONS(zeros.begin(), zeros.begin(24), v.begin(24), v.end());
    state._TP = 2;
    v = o.render(o.cursor() + 38);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(ones.begin(), ones.begin(16), v.begin(), v.begin(16));
    BOOST_REQUIRE_EQUAL_COLLECTIONS(zeros.begin(), zeros.begin(16), v.begin(16), v.begin(32));
    BOOST_REQUIRE_EQUAL_COLLECTIONS(ones.begin(), ones.begin(6), v.begin(32), v.end());
    state._TP = 1;
    v = o.render(o.cursor() + 19);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(ones.begin(), ones.begin(2), v.begin(), v.begin(2));
    BOOST_REQUIRE_EQUAL_COLLECTIONS(zeros.begin(), zeros.begin(8), v.begin(2), v.begin(10));
    BOOST_REQUIRE_EQUAL_COLLECTIONS(ones.begin(), ones.begin(8), v.begin(10), v.begin(18));
    BOOST_REQUIRE_EQUAL_COLLECTIONS(zeros.begin(), zeros.begin(1), v.begin(18), v.end());
}

BOOST_FIXTURE_TEST_CASE(smallerBlocksThanPeriod, F) {
    _config._atomSize = 1;
    State state(_config);
    state._TP = 5;
    Tone o(_config, state);
    BUF(24, 1, ones)
    BUF(24, 0, zeros)
    View<int> v = o.render(4);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(ones.begin(), ones.begin(4), v.begin(), v.end());
    v = o.render(o.cursor() + 4);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(ones.begin(), ones.begin(1), v.begin(), v.begin(1));
    BOOST_REQUIRE_EQUAL_COLLECTIONS(zeros.begin(), zeros.begin(3), v.begin(1), v.end());
    v = o.render(o.cursor() + 3);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(zeros.begin(), zeros.begin(2), v.begin(), v.begin(2));
    BOOST_REQUIRE_EQUAL_COLLECTIONS(ones.begin(), ones.begin(1), v.begin(2), v.end());
    v = o.render(o.cursor() + 4);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(ones.begin(), ones.begin(4), v.begin(), v.end());
    v = o.render(o.cursor() + 5);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(zeros.begin(), zeros.begin(5), v.begin(), v.end());
    v = o.render(o.cursor() + 1);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(ones.begin(), ones.begin(1), v.begin(), v.end());
}

BOOST_AUTO_TEST_SUITE_END()
