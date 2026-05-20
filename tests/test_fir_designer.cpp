#include "dsp/FirDesigner.h"

#include <catch2/catch_test_macros.hpp>

using namespace openref::dsp;

TEST_CASE("FIR designers return requested lengths")
{
    const std::vector<MagnitudePoint> curve {
        { 20.0, 0.0 },
        { 1000.0, 3.0 },
        { 20000.0, -3.0 },
    };
    REQUIRE(designLinearPhaseFir(curve, 48000.0, 512).size() == 512);
    REQUIRE(designMinimumPhaseFir(curve, 48000.0, 512).size() == 512);
}
