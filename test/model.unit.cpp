#include "doctest.h"
#include <model.h>

TEST_CASE("Test the silly Model") {
    LumpedMass m(20, 10, 5);
    m.stepForward(3600, 0.0);
    CHECK(m.temperature == 20);
    m.stepForward(3600, 1.0);
    CHECK(m.temperature == doctest::Approx(92));
}
