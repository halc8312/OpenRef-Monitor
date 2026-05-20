#include "Metering.h"

#include <algorithm>
#include <cmath>

namespace openref::dsp {

void PeakMeter::reset() noexcept
{
    current = {};
}

void PeakMeter::process(const float* samples, std::size_t count) noexcept
{
    auto peak = current.peak * 0.95f;
    auto clipped = current.clipped;
    for (std::size_t i = 0; i < count; ++i) {
        const auto sample = samples[i];
        const auto absSample = std::abs(sample);
        peak = std::max(peak, absSample);
        clipped = clipped || absSample >= 0.999f;
    }
    current = { peak, clipped };
}

} // namespace openref::dsp
