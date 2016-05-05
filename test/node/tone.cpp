#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>

#define BOOST_TEST_MODULE Tone

#include "../../src/buf.h"
#include "../../src/node/tone.h"
#include "../../src/state.h"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(minPeriod) {
    State state;
    state._midiNote = 167;
    Tone tone(&state);
    View<int> v = tone.render(17);
    int hmm[] = {1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1};
    BOOST_REQUIRE_EQUAL_COLLECTIONS(hmm, hmm + 17, v.begin(), v.end());
}
