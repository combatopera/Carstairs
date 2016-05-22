#include "../../src/util/buf.h"

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <array>
#include <cmath>

BOOST_AUTO_TEST_SUITE(TestBuffer)

BOOST_AUTO_TEST_CASE(mirror) {
    Buffer<int> buf("mirror", 11);
    buf.range();
    buf.mirror();
    std::array<int, 11> expected {0, 1, 2, 3, 4, 5, 4, 3, 2, 1, 0};
    BOOST_REQUIRE_EQUAL_COLLECTIONS(expected.begin(), expected.end(), buf.begin(), buf.end());
}

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
    for (int i = 0; i < 50; ++i) {
        BOOST_REQUIRE_EQUAL(buf.at(i), buf.at(100 - i));
    }
}

BOOST_AUTO_TEST_CASE(sinc) {
    Buffer<double> buf("sinc", 101);
    buf.range();
    buf.add(-50);
    buf.sinc();
    // Same way numpy does it:
    BOOST_REQUIRE_EQUAL(sin(M_PI * -10) / (M_PI * -10), buf.at(40));
    BOOST_REQUIRE_EQUAL(sin(M_PI * -1) / (M_PI * -1), buf.at(49));
    BOOST_REQUIRE_EQUAL(1, buf.at(50));
    BOOST_REQUIRE_EQUAL(sin(M_PI) / M_PI, buf.at(51));
    BOOST_REQUIRE_EQUAL(sin(M_PI * 10) / (M_PI * 10), buf.at(60));
    for (int i = 0; i < 50; ++i) {
        BOOST_REQUIRE_EQUAL(buf.at(i), buf.at(100 - i));
    }
}

BOOST_AUTO_TEST_CASE(pad) {
    Buffer<double> buf("pad", 5);
    buf.range();
    buf.add(1);
    buf.pad(2, 3, 8);
    std::array<double, 10> expected {8, 8, 1, 2, 3, 4, 5, 8, 8, 8};
    BOOST_REQUIRE_EQUAL_COLLECTIONS(expected.begin(), expected.end(), buf.begin(), buf.end());
}

BOOST_AUTO_TEST_CASE(integrate) {
    Buffer<double> buf("integrate", 3);
    buf.range();
    buf.add(1);
    buf.integrate();
    std::array<double, 3> expected {1, 1 + 2, 1 + 2 + 3};
    BOOST_REQUIRE_EQUAL_COLLECTIONS(expected.begin(), expected.end(), buf.begin(), buf.end());
}

BOOST_AUTO_TEST_CASE(fft) {
    size_t n = 1024;
    Buffer<std::complex<double>> buf("fft", n);
    buf.range();
    buf.fft();
    buf.ifft();
    for (index_t i = 0; i < n; ++i) {
        BOOST_REQUIRE_CLOSE_FRACTION(i, buf.at(i).real(), 1e-14);
        BOOST_REQUIRE_SMALL(buf.at(i).imag(), 1e-9);
    }
}

BOOST_AUTO_TEST_CASE(differentiate) {
    Buffer<double> buf("differentiate", 10);
    buf.range();
    buf.put(5, 0);
    buf.differentiate(2);
    std::array<double, 10> expected {-2, 1, 1, 1, 1, -4, 6, 1, 1, 1};
    BOOST_REQUIRE_EQUAL_COLLECTIONS(expected.begin(), expected.end(), buf.begin(), buf.end());
}

BOOST_AUTO_TEST_CASE(fillWidening) {
    Buffer<double> buf("fillWidening", 10);
    Buffer<std::complex<double>> cbuf("fillWidening", 10);
    buf.range();
    cbuf.fillWidening(buf.begin());
    for (int i = 0; i < 10; ++i) {
        BOOST_CHECK_EQUAL(i, cbuf.at(i).real());
        BOOST_CHECK_EQUAL(0, cbuf.at(i).imag());
    }
    buf.fillReal(cbuf.begin());
    std::array<double, 10> expected {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    BOOST_REQUIRE_EQUAL_COLLECTIONS(expected.begin(), expected.end(), buf.begin(), buf.end());
}

BOOST_AUTO_TEST_CASE(rceps) {
    Buffer<double> buf("rceps", 10);
    buf.range();
    {
        Buffer<std::complex<double>> fftAppliance("fftAppliance");
        buf.rceps(fftAppliance, 0);
    }
    for (int i = 1; i < 5; ++i) {
        BOOST_CHECK_EQUAL(buf.at(i), buf.at(10 - i));
    }
    Buffer<double> rceps("rceps");
    rceps.snapshot(buf);
    {
        Buffer<std::complex<double>> fftAppliance("fftAppliance");
        buf.minPhaseFromRceps(fftAppliance);
        buf.rceps(fftAppliance, 0);
    }
    for (int i = 0; i < 10; ++i) {
        BOOST_CHECK_CLOSE_FRACTION(rceps.at(i), buf.at(i), 1e-14);
    }
}

BOOST_AUTO_TEST_SUITE_END()
