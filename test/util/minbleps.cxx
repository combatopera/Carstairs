#include "../../src/util/minbleps.h"

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>

BOOST_AUTO_TEST_SUITE(TestMinBLEPs)

BOOST_AUTO_TEST_CASE(minBLEPSize) {
    Config config(6);
    config._nominalClock = 15;
    MinBLEPs minBLEPs(&config);
    BOOST_REQUIRE_EQUAL(5, minBLEPs._minBLEPCount);
    BOOST_REQUIRE_EQUAL(512, minBLEPs._minBLEPs.limit());
    BOOST_REQUIRE_EQUAL(103, minBLEPs.minBLEPSize(0));
    BOOST_REQUIRE_EQUAL(103, minBLEPs.minBLEPSize(1));
    BOOST_REQUIRE_EQUAL(102, minBLEPs.minBLEPSize(2));
    BOOST_REQUIRE_EQUAL(102, minBLEPs.minBLEPSize(3));
    BOOST_REQUIRE_EQUAL(102, minBLEPs.minBLEPSize(4));
}

BOOST_AUTO_TEST_CASE(pcmXToNaiveX) {
    Config config(6);
    config._nominalClock = 15;
    MinBLEPs minBLEPs(&config);
    BOOST_REQUIRE_EQUAL(1, minBLEPs.pcmXToNaiveX(1));
    BOOST_REQUIRE_EQUAL(3, minBLEPs.pcmXToNaiveX(2));
    BOOST_REQUIRE_EQUAL(6, minBLEPs.pcmXToNaiveX(3));
    BOOST_REQUIRE_EQUAL(8, minBLEPs.pcmXToNaiveX(4));
    BOOST_REQUIRE_EQUAL(11, minBLEPs.pcmXToNaiveX(5));
    BOOST_REQUIRE_EQUAL(13, minBLEPs.pcmXToNaiveX(6));
}

BOOST_AUTO_TEST_SUITE_END()
