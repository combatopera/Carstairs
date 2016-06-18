#include "../../src/util/minbleps.h"

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>

#include "../../src/util/paster.h"

BOOST_AUTO_TEST_SUITE(TestMinBLEPs)

struct F {

    Config _config;

    int _pcmRate = 6;

    F() {
        _config._atomSize = _config.YM2149_ATOM_SIZE;
        _config._nominalClock = 15;
        _config._transition = .05;
        _config._stopband = 0;
        _config._minBLEPCount = 5;
    }

};

BOOST_FIXTURE_TEST_CASE(checkFixture, F) {
    BOOST_REQUIRE_EQUAL(.475, _config.cutoff());
    BOOST_REQUIRE_EQUAL(_config._minBLEPCount, _config.idealMinBLEPCount(_pcmRate));
}

BOOST_FIXTURE_TEST_CASE(BLI, F) {
    MinBLEPs minBLEPs(_config, _pcmRate);
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
    MinBLEPs minBLEPs1(_config, _pcmRate);
    _config._rcepsAddBeforeLog = 0;
    MinBLEPs minBLEPs0(_config, _pcmRate);
    _config._rcepsAddBeforeLog = .5;
    MinBLEPs minBLEPs2(_config, _pcmRate);
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
    MinBLEPs minBLEPs(_config, _pcmRate);
    BOOST_REQUIRE_EQUAL(512, minBLEPs._minBLEPs.limit());
    for (int i = 0; i < 3; ++i) {
        BOOST_CHECK_SMALL(minBLEPs._minBLEPs.at(i), 1e-4f);
    }
    for (int i = 500; i < 512; ++i) {
        BOOST_CHECK_CLOSE_FRACTION(1, minBLEPs._minBLEPs.at(i), 1e-4f);
    }
}

BOOST_FIXTURE_TEST_CASE(minBLEPSize, F) {
    MinBLEPs minBLEPs(_config, _pcmRate);
    BOOST_REQUIRE_EQUAL(5, minBLEPs._minBLEPCount);
    BOOST_REQUIRE_EQUAL(512, minBLEPs._minBLEPs.limit());
    Paster paster(minBLEPs);
    BOOST_REQUIRE_EQUAL(103, paster.minBLEPSize(0));
    BOOST_REQUIRE_EQUAL(103, paster.minBLEPSize(1));
    BOOST_REQUIRE_EQUAL(102, paster.minBLEPSize(2));
    BOOST_REQUIRE_EQUAL(102, paster.minBLEPSize(3));
    BOOST_REQUIRE_EQUAL(102, paster.minBLEPSize(4));
}

BOOST_FIXTURE_TEST_CASE(getNaiveCursor, F) {
    MinBLEPs minBLEPs(_config, _pcmRate);
    BOOST_REQUIRE_EQUAL(1, minBLEPs.getNaiveCursor(1));
    BOOST_REQUIRE_EQUAL(3, minBLEPs.getNaiveCursor(2));
    BOOST_REQUIRE_EQUAL(6, minBLEPs.getNaiveCursor(3));
    BOOST_REQUIRE_EQUAL(8, minBLEPs.getNaiveCursor(4));
    BOOST_REQUIRE_EQUAL(11, minBLEPs.getNaiveCursor(5));
    BOOST_REQUIRE_EQUAL(13, minBLEPs.getNaiveCursor(6));
}

BOOST_FIXTURE_TEST_CASE(getNaiveCursorRoundingError, F) {
    _config._nominalClock = 2000000;
    _config._atomSize = 1;
    _pcmRate = 44100;
    MinBLEPs minBLEPs(_config, _pcmRate);
    Paster p(minBLEPs);
    BOOST_CHECK_EQUAL(94546431, p.getPcmMark(535977500)); // Logically correct.
    BOOST_CHECK_EQUAL(535977501, minBLEPs.getNaiveCursor(94546432));
}

BOOST_FIXTURE_TEST_CASE(getNaiveCursorRoundingError2, F) {
    _config._nominalClock = 2000000;
    _config._atomSize = 1;
    _pcmRate = 48000;
    MinBLEPs minBLEPs(_config, _pcmRate);
    Paster p(minBLEPs);
    BOOST_CHECK_EQUAL(1537536, p.getPcmMark(8008000)); // Logically correct.
    BOOST_CHECK_EQUAL(8008001, minBLEPs.getNaiveCursor(1537537));
}

BOOST_AUTO_TEST_SUITE_END()
