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

#include "config.h"

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>

BOOST_AUTO_TEST_SUITE(TestConfig)

BOOST_AUTO_TEST_CASE(evenEmpiricalOrder) {
    Config config;
    config._transition = .05;
    BOOST_REQUIRE_EQUAL(80, config.evenEmpiricalOrder());
    config._transition = .0505;
    BOOST_REQUIRE_EQUAL(80, config.evenEmpiricalOrder());
    config._transition = .051;
    BOOST_REQUIRE_EQUAL(78, config.evenEmpiricalOrder());
    config._transition = .0495;
    BOOST_REQUIRE_EQUAL(80, config.evenEmpiricalOrder());
    config._transition = .049;
    BOOST_REQUIRE_EQUAL(82, config.evenEmpiricalOrder());
}

BOOST_AUTO_TEST_SUITE_END()
