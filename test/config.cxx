#include "../src/config.h"
 
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
