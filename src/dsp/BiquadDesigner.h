#pragma once

#include "Biquad.h"

namespace openref::dsp {

struct FilterSpec {
    FilterType type { FilterType::peaking };
    double frequencyHz { 1000.0 };
    double gainDb { 0.0 };
    double q { 0.70710678118 };
};

[[nodiscard]] BiquadCoefficients designBiquad(const FilterSpec& spec, double sampleRate);

} // namespace openref::dsp
