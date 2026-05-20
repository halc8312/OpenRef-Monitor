#include "dsp/Biquad.h"
#include "dsp/BiquadDesigner.h"
#include "dsp/ResponseAnalyzer.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using namespace openref::dsp;

TEST_CASE("Biquad peaking response is near requested gain")
{
    Biquad filter;
    filter.setCoefficients(designBiquad({ FilterType::peaking, 1000.0, 6.0, 1.0 }, 48000.0));
    REQUIRE_THAT(filter.magnitudeDb(1000.0, 48000.0), Catch::Matchers::WithinAbs(6.0, 0.1));
}

TEST_CASE("Safe headroom is one dB below maximum boost")
{
    const std::vector<FilterSpec> filters {
        { FilterType::peaking, 1000.0, 6.0, 1.0 },
    };
    REQUIRE(calculateSafeHeadroomDb(filters, 48000.0) < -6.5);
    REQUIRE(calculateSafeHeadroomDb(filters, 48000.0) > -7.5);
}
