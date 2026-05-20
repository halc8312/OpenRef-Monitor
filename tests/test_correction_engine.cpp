#include "dsp/CorrectionEngine.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <cmath>

using namespace openref::dsp;

TEST_CASE("Extra filters affect response and processing")
{
    CorrectionEngine engine;
    engine.prepare(48000.0, {}, {});

    const FilterSpec extraFilters[] {
        { FilterType::peaking, 1000.0, 6.0, 1.0 },
    };
    engine.setExtraFilters(extraFilters, 1);
    REQUIRE_THAT(engine.responseDb(1000.0, 0), Catch::Matchers::WithinAbs(6.0, 0.1));

    std::vector<float> left(32, 0.1f);
    std::vector<float> right = left;
    engine.process(left.data(), right.data(), left.size(), 1.0f, false);
    REQUIRE(std::abs(left[10] - 0.1f) > 0.0001f);
}

TEST_CASE("Mono translation sums processed channels")
{
    CorrectionEngine engine;
    engine.prepare(48000.0, {}, {});

    std::vector<float> left { 1.0f, 0.5f };
    std::vector<float> right { -1.0f, -0.5f };
    engine.process(left.data(), right.data(), left.size(), 1.0f, true);

    REQUIRE_THAT(left[0], Catch::Matchers::WithinAbs(0.0f, 0.0001f));
    REQUIRE_THAT(right[0], Catch::Matchers::WithinAbs(0.0f, 0.0001f));
    REQUIRE_THAT(left[1], Catch::Matchers::WithinAbs(0.0f, 0.0001f));
    REQUIRE_THAT(right[1], Catch::Matchers::WithinAbs(0.0f, 0.0001f));
}
