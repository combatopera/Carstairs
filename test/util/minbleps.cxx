#include "../../src/util/minbleps.h"

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>

BOOST_AUTO_TEST_SUITE(TestMinBLEPs)

struct F {

    Config _config {6};

    F() {
        _config._nominalClock = 15;
    }

};

BOOST_FIXTURE_TEST_CASE(BLI, F) {
    MinBLEPs minBLEPs(&_config);
    BOOST_REQUIRE_EQUAL(401, minBLEPs._BLI.limit());
    for (int i = 0; i < 200; ++i) {
        BOOST_REQUIRE_EQUAL(minBLEPs._BLI.at(i), minBLEPs._BLI.at(400 - i));
    }
    BOOST_REQUIRE_CLOSE_FRACTION(.19, minBLEPs._BLI.at(200), 1e-15);
}

BOOST_FIXTURE_TEST_CASE(minBLEPSize, F) {
    MinBLEPs minBLEPs(&_config);
    BOOST_REQUIRE_EQUAL(5, minBLEPs._minBLEPCount);
    BOOST_REQUIRE_EQUAL(512, minBLEPs._minBLEPs.limit());
    BOOST_REQUIRE_EQUAL(103, minBLEPs.minBLEPSize(0));
    BOOST_REQUIRE_EQUAL(103, minBLEPs.minBLEPSize(1));
    BOOST_REQUIRE_EQUAL(102, minBLEPs.minBLEPSize(2));
    BOOST_REQUIRE_EQUAL(102, minBLEPs.minBLEPSize(3));
    BOOST_REQUIRE_EQUAL(102, minBLEPs.minBLEPSize(4));
}

BOOST_FIXTURE_TEST_CASE(pcmXToNaiveX, F) {
    MinBLEPs minBLEPs(&_config);
    BOOST_REQUIRE_EQUAL(1, minBLEPs.pcmXToNaiveX(1));
    BOOST_REQUIRE_EQUAL(3, minBLEPs.pcmXToNaiveX(2));
    BOOST_REQUIRE_EQUAL(6, minBLEPs.pcmXToNaiveX(3));
    BOOST_REQUIRE_EQUAL(8, minBLEPs.pcmXToNaiveX(4));
    BOOST_REQUIRE_EQUAL(11, minBLEPs.pcmXToNaiveX(5));
    BOOST_REQUIRE_EQUAL(13, minBLEPs.pcmXToNaiveX(6));
}

BOOST_AUTO_TEST_SUITE_END()
