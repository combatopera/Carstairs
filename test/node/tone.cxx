#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>

#include "../../src/config.h"
#include "../../src/node.h"

#define BOOST_TEST_MODULE Tone

#include "../../src/buf.h"
#include "../../src/node/tone.h"
#include "../../src/state.h"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(works) {
    Config config(8);
    State state;
    state._TP = 3;
    Tone o(&config, &state);
    Buffer<int> ones("ones"), zeros("zeros");
    ones.setLimit(24);
    zeros.setLimit(24);
    ones.fill(0, 24, 1);
    zeros.fill(0, 24, 0);
    View<int> v = o.render(96);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(ones.begin(), ones.end(), v.begin(), v.begin() + 24);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(zeros.begin(), zeros.end(), v.begin() + 24, v.begin() + 48);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(ones.begin(), ones.end(), v.begin() + 48, v.begin() + 72);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(zeros.begin(), zeros.end(), v.begin() + 72, v.end());
    v = o.render(v.limit() + 48);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(ones.begin(), ones.end(), v.begin(), v.begin() + 24);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(zeros.begin(), zeros.end(), v.begin() + 24, v.end());
}

BOOST_AUTO_TEST_CASE(resume) {
    Config config(8);
    State state;
    state._TP = 3;
    Tone o(&config, &state);
    Buffer<int> ones("ones"), zeros("zeros");
    ones.setLimit(24);
    zeros.setLimit(24);
    ones.fill(0, 24, 1);
    zeros.fill(0, 24, 0);
    View<int> v = o.render(25);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(ones.begin(), ones.end(), v.begin(), v.begin() + 24);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(zeros.begin(), zeros.begin() + 1, v.begin() + 24, v.end());
    v = o.render(v.limit() + 24);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(zeros.begin(), zeros.begin() + 23, v.begin(), v.begin() + 23);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(ones.begin(), ones.begin() + 1, v.begin() + 23, v.end());
}

BOOST_AUTO_TEST_CASE(carry) {
    Config config(8);
    State state;
    state._TP = 1;
    int size = 3 * 8 + 1;
    Tone refOsc(&config, &state); // Must stay in scope for ref to be valid.
    View<int> ref = refOsc.render(size);
    for (int n = 0; n <= size; ++n) {
        Tone o(&config, &state);
        View<int> v = o.render(n);
        BOOST_REQUIRE_EQUAL_COLLECTIONS(ref.begin(), ref.begin() + v.limit(), v.begin(), v.end());
        v = o.render(size);
        BOOST_REQUIRE_EQUAL_COLLECTIONS(ref.end() - v.limit(), ref.end(), v.begin(), v.end());
    }
}

BOOST_AUTO_TEST_CASE(endExistingStepAtEndOfBlock) {
    Config config(8);
    State state;
    state._TP = 1;
    Tone o(&config, &state);
    Buffer<int> ones("ones"), zeros("zeros");
    ones.setLimit(4);
    zeros.setLimit(4);
    ones.fill(0, 4, 1);
    zeros.fill(0, 4, 0);
    View<int> v = o.render(4);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(ones.begin(), ones.end(), v.begin(), v.end());
    v = o.render(8);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(ones.begin(), ones.end(), v.begin(), v.end());
    v = o.render(12);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(zeros.begin(), zeros.end(), v.begin(), v.end());
}

BOOST_AUTO_TEST_CASE(increasePeriodOnBoundary) {
    Config config(8);
    State state;
    state._TP = 1;
    Tone o(&config, &state);
    Buffer<int> ones("ones"), zeros("zeros");
    ones.setLimit(24);
    zeros.setLimit(24);
    ones.fill(0, 24, 1);
    zeros.fill(0, 24, 0);
    View<int> v = o.render(16);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(ones.begin(), ones.begin() + 8, v.begin(), v.begin() + 8);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(zeros.begin(), zeros.begin() + 8, v.begin() + 8, v.end());
    state._TP = 2;
    v = o.render(o.cursor() + 31);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(ones.begin(), ones.begin() + 16, v.begin(), v.begin() + 16);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(zeros.begin(), zeros.begin() + 15, v.begin() + 16, v.end());
    state._TP = 3;
    v = o.render(o.cursor() + 34);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(zeros.begin(), zeros.begin() + 9, v.begin(), v.begin() + 9);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(ones.begin(), ones.begin() + 24, v.begin() + 9, v.begin() + 33);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(zeros.begin(), zeros.begin() + 1, v.begin() + 33, v.end());
}
