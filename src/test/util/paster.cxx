// Copyright 2016 Andrzej Cichocki

// This file is part of Carstairs.
//
// Carstairs is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Carstairs is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Carstairs.  If not, see <http://www.gnu.org/licenses/>.

#include "util/paster.h"

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <array>

BOOST_AUTO_TEST_SUITE(TestPaster)

struct F {

    struct BLEPs: public Buffer<float> {
        BLEPs()
                : Buffer {"_minBLEPs", 18} {
            range();
            add(1); // [1,6,11,16], [2,7,12,17], [3,8,13,18], [4,9,14], [5,10,15]
        }
    } _minBLEPs;

    Paster _paster {6. / 15, _minBLEPs, 5};

    Buffer<float> _derivative {"_derivative", 15};

    Buffer<float> _pcm {"_pcm", 10};

};

BOOST_FIXTURE_TEST_CASE(pasteMulti0, F) {
    {
        _derivative.zero();
        _derivative.put(0, 3);
        _paster.pasteMulti(_derivative, 0, 0, _pcm, 0, 2);
        std::array<float, 10> expected {5, 20, 35, 50, 5, 5, 5, 5, 5, 5};
        BOOST_REQUIRE_EQUAL_COLLECTIONS(expected.begin(), expected.end(), _pcm.begin(), _pcm.end());
    }
    {
        _derivative.zero();
        _derivative.put(5, 3);
        _paster.pasteMulti(_derivative, 0, 0, _pcm, 0, 2);
        std::array<float, 10> expected {2, 2, 5, 20, 35, 50, 5, 5, 5, 5};
        BOOST_REQUIRE_EQUAL_COLLECTIONS(expected.begin(), expected.end(), _pcm.begin(), _pcm.end());
    }
    {
        _derivative.zero();
        _derivative.put(2, 3);
        _paster.pasteMulti(_derivative, 3, 1, _pcm, 0, 2);
        std::array<float, 10> expected {2, 5, 20, 35, 50, 5, 5, 5, 5, 5};
        BOOST_REQUIRE_EQUAL_COLLECTIONS(expected.begin(), expected.end(), _pcm.begin(), _pcm.end());
    }
}

BOOST_FIXTURE_TEST_CASE(pasteMulti1, F) {
    {
        _derivative.zero();
        _derivative.put(2, 3); // PCM mark .8 i.e. distance .2 (minBLEP 1) to index 1.
        _paster.pasteMulti(_derivative, 0, 0, _pcm, 0, 2);
        std::array<float, 10> expected {2, 8, 23, 38, 53, 5, 5, 5, 5, 5};
        BOOST_REQUIRE_EQUAL_COLLECTIONS(expected.begin(), expected.end(), _pcm.begin(), _pcm.end());
    }
    {
        _derivative.zero();
        _derivative.put(7, 3);
        _paster.pasteMulti(_derivative, 0, 0, _pcm, 0, 2);
        std::array<float, 10> expected {2, 2, 2, 8, 23, 38, 53, 5, 5, 5};
        BOOST_REQUIRE_EQUAL_COLLECTIONS(expected.begin(), expected.end(), _pcm.begin(), _pcm.end());
    }
}

BOOST_FIXTURE_TEST_CASE(pasteMulti3, F) {
    {
        _derivative.zero();
        _derivative.put(1, 3); // PCM mark .4 i.e. distance .6 (minBLEP 3) to index 1.
        _paster.pasteMulti(_derivative, 0, 0, _pcm, 0, 2);
        std::array<float, 10> expected {2, 14, 29, 44, 5, 5, 5, 5, 5, 5};
        BOOST_REQUIRE_EQUAL_COLLECTIONS(expected.begin(), expected.end(), _pcm.begin(), _pcm.end());
    }
    {
        _derivative.zero();
        _derivative.put(11, 3);
        _paster.pasteMulti(_derivative, 0, 0, _pcm, 0, 2);
        std::array<float, 10> expected {2, 2, 2, 2, 2, 14, 29, 44, 5, 5};
        BOOST_REQUIRE_EQUAL_COLLECTIONS(expected.begin(), expected.end(), _pcm.begin(), _pcm.end());
    }
}

BOOST_FIXTURE_TEST_CASE(twice, F) {
    _derivative.zero();
    _derivative.put(1, 3);
    _derivative.put(11, 2);
    _paster.pasteMulti(_derivative, 0, 0, _pcm, 0, 2);
    std::array<float, 10> expected {2, 14, 29, 44, 5, 13, 23, 33, 7, 7};
    BOOST_REQUIRE_EQUAL_COLLECTIONS(expected.begin(), expected.end(), _pcm.begin(), _pcm.end());
}

BOOST_FIXTURE_TEST_CASE(overlap, F) {
    _derivative.zero();
    _derivative.put(1, 3);
    _derivative.put(6, 2);
    _paster.pasteMulti(_derivative, 0, 0, _pcm, 0, 2);
    std::array<float, 10> expected {2, 14, 29, 52, 23, 33, 7, 7, 7, 7};
    BOOST_REQUIRE_EQUAL_COLLECTIONS(expected.begin(), expected.end(), _pcm.begin(), _pcm.end());
}

BOOST_AUTO_TEST_SUITE_END()
