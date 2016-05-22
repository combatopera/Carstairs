#include "../../src/util/minbleps.h"

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>

BOOST_AUTO_TEST_SUITE(TestMinBLEPs)

struct F {

    Config _config {6};

    F() {
        _config._atomSize = Config::YM2149_ATOM_SIZE;
        _config._nominalClock = 15;
        _config._cutoff = .475;
        _config._transition = .05;
        BOOST_REQUIRE_EQUAL(_config._minBLEPCount = 5, _config.idealMinBLEPCount());
    }

};

BOOST_FIXTURE_TEST_CASE(BLI, F) {
    MinBLEPs minBLEPs(&_config);
    BOOST_REQUIRE_EQUAL(401, minBLEPs._BLI.limit());
    for (int i = 0; i < 200; ++i) {
        BOOST_REQUIRE_EQUAL(minBLEPs._BLI.at(i), minBLEPs._BLI.at(400 - i));
    }
    BOOST_REQUIRE_CLOSE_FRACTION(.19, minBLEPs._BLI.at(200), 1e-15);
    minBLEPs._BLI.integrate();
    for (int i = 380; i < 390; ++i) {
        BOOST_CHECK_CLOSE_FRACTION(1, minBLEPs._BLI.at(i), 1e-4);
    }
    for (int i = 390; i < 400; ++i) {
        BOOST_CHECK_CLOSE_FRACTION(1, minBLEPs._BLI.at(i), 1e-5);
    }
    BOOST_CHECK_CLOSE_FRACTION(1, minBLEPs._BLI.at(400), 1e-6);
}

BOOST_FIXTURE_TEST_CASE(realCepstrum, F) {
    BOOST_REQUIRE_EQUAL(1e-50, _config._rcepsAddBeforeLog);
    MinBLEPs minBLEPs1(&_config);
    _config._rcepsAddBeforeLog = 0;
    MinBLEPs minBLEPs0(&_config);
    _config._rcepsAddBeforeLog = .5;
    MinBLEPs minBLEPs2(&_config);
    BOOST_REQUIRE_EQUAL(512, minBLEPs0._realCepstrum.limit());
    BOOST_REQUIRE_EQUAL(512, minBLEPs1._realCepstrum.limit());
    BOOST_REQUIRE_EQUAL(512, minBLEPs2._realCepstrum.limit());
    for (int i = 0; i < 512; ++i) {
        BOOST_CHECK_SMALL(minBLEPs0._realCepstrum.at(i).imag(), 1e-5);
        BOOST_CHECK_SMALL(minBLEPs1._realCepstrum.at(i).imag(), 1e-5);
        BOOST_CHECK_SMALL(minBLEPs2._realCepstrum.at(i).imag(), 1e-14);
    }
}

BOOST_FIXTURE_TEST_CASE(minBLEPs, F) {
    MinBLEPs minBLEPs(&_config);
    BOOST_REQUIRE_EQUAL(512, minBLEPs._minBLEPs.limit());
    for (int i = 0; i < 3; ++i) {
        BOOST_CHECK_SMALL(minBLEPs._minBLEPs.at(i), 1e-4f);
    }
    for (int i = 500; i < 512; ++i) {
        BOOST_CHECK_CLOSE_FRACTION(1, minBLEPs._minBLEPs.at(i), 1e-4f);
    }
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
