#include "SmoothedBypass.h"

#include <algorithm>
#include <cmath>

namespace openref::dsp {

void SmoothedBypass::prepare(double sampleRate, double rampMs) noexcept
{
    const auto samples = std::max(1.0, sampleRate * rampMs / 1000.0);
    step = static_cast<float>(1.0 / samples);
}

void SmoothedBypass::setEnabled(bool enabled) noexcept
{
    target = enabled ? 1.0f : 0.0f;
}

float SmoothedBypass::next() noexcept
{
    if (std::abs(current - target) <= step) {
        current = target;
    } else if (current < target) {
        current += step;
    } else {
        current -= step;
    }
    return std::clamp(current, 0.0f, 1.0f);
}

} // namespace openref::dsp
