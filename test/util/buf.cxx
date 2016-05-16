#include "../../src/util/buf.h"

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <cmath>
#include <complex>

#include "../../src/util/util.h"

BOOST_AUTO_TEST_SUITE(TestBuffer)

BOOST_AUTO_TEST_CASE(blackman) {
    Buffer<double> buf("blackman", 101);
    buf.blackman();
    BOOST_REQUIRE_SMALL(buf.at(0), 1e-16);
    BOOST_REQUIRE_SMALL(buf.at(1), 1e-3);
    BOOST_REQUIRE_CLOSE_FRACTION(1, buf.at(49), 1e-2);
    BOOST_REQUIRE_CLOSE_FRACTION(1, buf.at(50), 1e-15);
    BOOST_REQUIRE_CLOSE_FRACTION(1, buf.at(51), 1e-2);
    BOOST_REQUIRE_SMALL(buf.at(99), 1e-3);
    BOOST_REQUIRE_SMALL(buf.at(100), 1e-16);
    BOOST_REQUIRE_LT(buf.at(0), buf.at(1));
    BOOST_REQUIRE_LT(buf.at(49), buf.at(50));
    BOOST_REQUIRE_GT(buf.at(50), buf.at(51));
    BOOST_REQUIRE_GT(buf.at(99), buf.at(100));
}

BOOST_AUTO_TEST_CASE(sinc) {
    Buffer<double> buf("sinc", 11);
    buf.range();
    buf.sinc();
    BOOST_REQUIRE_EQUAL(1, buf.at(0));
    BOOST_REQUIRE_EQUAL(sin(M_PI) / M_PI, buf.at(1));
    BOOST_REQUIRE_EQUAL(sin(M_PI * 10) / (M_PI * 10), buf.at(10));
}

BOOST_AUTO_TEST_CASE(zeroPad) {
    Buffer<double> buf("zeroPad", 3);
    buf.range();
    buf.add(1);
    buf.zeroPad(5, 2);
    double expected[] {0, 0, 0, 0, 0, 1, 2, 3, 0, 0};
    BOOST_REQUIRE_EQUAL_COLLECTIONS(expected, expected + 10, buf.begin(), buf.end());
}

BOOST_AUTO_TEST_CASE(integrate) {
    Buffer<double> buf("integrate", 3);
    buf.range();
    buf.add(1);
    buf.integrate();
    index_t i = 0;
    BOOST_REQUIRE_EQUAL(1, buf.at(i++));
    BOOST_REQUIRE_EQUAL(1 + 2, buf.at(i++));
    BOOST_REQUIRE_EQUAL(1 + 2 + 3, buf.at(i++));
    BOOST_REQUIRE_EQUAL(buf.limit(), i);
}

BOOST_AUTO_TEST_CASE(fft) {
    size_t n = 1024;
    Buffer<std::complex<double>> buf("fft", n);
    buf.range();
    buf.fft();
    buf.ifft();
    for (index_t i = 0; i < n; ++i) {
        BOOST_REQUIRE_CLOSE_FRACTION(i, buf.at(i).real() / double(n), 1e-14);
        BOOST_REQUIRE_SMALL(buf.at(i).imag(), 1e-9);
    }
}

BOOST_AUTO_TEST_SUITE_END()
