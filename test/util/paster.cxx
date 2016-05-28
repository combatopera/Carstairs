#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <array>

#include "../../src/util/buf.h"
#include "../../src/util/minbleps.h"

BOOST_AUTO_TEST_SUITE(TestPaster)

struct F {

    struct BLEPs: public Buffer<float> {
        BLEPs()
                : Buffer {"_minBLEPs", 18} {
            range();
            add(1); // [1,6,11,16], [2,7,12,17], [3,8,13,18], [4,9,14], [5,10,15]
        }
    } _minBLEPs;

    MinBLEPs::Paster _paster {15, 6, _minBLEPs, 5};

    Buffer<float> _derivative {"_derivative", 15};

    Buffer<float> _pcm {"_pcm", 10};

};

BOOST_FIXTURE_TEST_CASE(pasteMulti0, F) {
    {
        _derivative.zero();
        _derivative.put(0, 3);
        _pcm.fill(2); // Context DC.
        _paster.pasteMulti(_derivative, 0, 0, _pcm);
        std::array<float, 10> expected {5, 20, 35, 50, 5, 5, 5, 5, 5, 5};
        BOOST_REQUIRE_EQUAL_COLLECTIONS(expected.begin(), expected.end(), _pcm.begin(), _pcm.end());
    }
    {
        _derivative.zero();
        _derivative.put(5, 3);
        _pcm.fill(2); // Context DC.
        _paster.pasteMulti(_derivative, 0, 0, _pcm);
        std::array<float, 10> expected {2, 2, 5, 20, 35, 50, 5, 5, 5, 5};
        BOOST_REQUIRE_EQUAL_COLLECTIONS(expected.begin(), expected.end(), _pcm.begin(), _pcm.end());
    }
    {
        _derivative.zero();
        _derivative.put(2, 3);
        _pcm.fill(2); // Context DC.
        _paster.pasteMulti(_derivative, 3, 1, _pcm);
        std::array<float, 10> expected {2, 5, 20, 35, 50, 5, 5, 5, 5, 5};
        BOOST_REQUIRE_EQUAL_COLLECTIONS(expected.begin(), expected.end(), _pcm.begin(), _pcm.end());
    }
}

BOOST_FIXTURE_TEST_CASE(pasteMulti1, F) {
    {
        _derivative.zero();
        _derivative.put(2, 3); // PCM mark .8 i.e. distance .2 (minBLEP 1) to index 1.
        _pcm.fill(2); // Context DC.
        _paster.pasteMulti(_derivative, 0, 0, _pcm);
        std::array<float, 10> expected {2, 8, 23, 38, 53, 5, 5, 5, 5, 5};
        BOOST_REQUIRE_EQUAL_COLLECTIONS(expected.begin(), expected.end(), _pcm.begin(), _pcm.end());
    }
    {
        _derivative.zero();
        _derivative.put(7, 3);
        _pcm.fill(2); // Context DC.
        _paster.pasteMulti(_derivative, 0, 0, _pcm);
        std::array<float, 10> expected {2, 2, 2, 8, 23, 38, 53, 5, 5, 5};
        BOOST_REQUIRE_EQUAL_COLLECTIONS(expected.begin(), expected.end(), _pcm.begin(), _pcm.end());
    }
}

BOOST_FIXTURE_TEST_CASE(pasteMulti3, F) {
    {
        _derivative.zero();
        _derivative.put(1, 3); // PCM mark .4 i.e. distance .6 (minBLEP 3) to index 1.
        _pcm.fill(2); // Context DC.
        _paster.pasteMulti(_derivative, 0, 0, _pcm);
        std::array<float, 10> expected {2, 14, 29, 44, 5, 5, 5, 5, 5, 5};
        BOOST_REQUIRE_EQUAL_COLLECTIONS(expected.begin(), expected.end(), _pcm.begin(), _pcm.end());
    }
    {
        _derivative.zero();
        _derivative.put(11, 3);
        _pcm.fill(2); // Context DC.
        _paster.pasteMulti(_derivative, 0, 0, _pcm);
        std::array<float, 10> expected {2, 2, 2, 2, 2, 14, 29, 44, 5, 5};
        BOOST_REQUIRE_EQUAL_COLLECTIONS(expected.begin(), expected.end(), _pcm.begin(), _pcm.end());
    }
}

BOOST_FIXTURE_TEST_CASE(twice, F) {
    _derivative.zero();
    _derivative.put(1, 3);
    _derivative.put(11, 2);
    _pcm.fill(2); // Context DC.
    _paster.pasteMulti(_derivative, 0, 0, _pcm);
    std::array<float, 10> expected {2, 14, 29, 44, 5, 13, 23, 33, 7, 7};
    BOOST_REQUIRE_EQUAL_COLLECTIONS(expected.begin(), expected.end(), _pcm.begin(), _pcm.end());
}

BOOST_FIXTURE_TEST_CASE(overlap, F) {
    _derivative.zero();
    _derivative.put(1, 3);
    _derivative.put(6, 2);
    _pcm.fill(2); // Context DC.
    _paster.pasteMulti(_derivative, 0, 0, _pcm);
    std::array<float, 10> expected {2, 14, 29, 52, 23, 33, 7, 7, 7, 7};
    BOOST_REQUIRE_EQUAL_COLLECTIONS(expected.begin(), expected.end(), _pcm.begin(), _pcm.end());
}

BOOST_AUTO_TEST_SUITE_END()
