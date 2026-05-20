#include "dsp/CorrectionEngine.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using namespace openref::dsp;

TEST_CASE("All-zero correction nulls with dry/wet fully wet")
{
    CorrectionEngine engine;
    engine.prepare(48000.0, { { FilterType::peaking, 1000.0, 0.0, 1.0 } }, {});
    std::vector<float> left { 0.0f, 0.25f, -0.5f, 0.75f };
    std::vector<float> right = left;
    engine.process(left.data(), right.data(), left.size(), 1.0f, false);
    REQUIRE_THAT(left[1], Catch::Matchers::WithinAbs(0.25f, 0.0001f));
    REQUIRE_THAT(left[2], Catch::Matchers::WithinAbs(-0.5f, 0.0001f));
}
