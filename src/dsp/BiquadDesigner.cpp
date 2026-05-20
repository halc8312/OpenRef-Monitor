#include "BiquadDesigner.h"

#include <algorithm>
#include <cmath>
#include <numbers>

namespace openref::dsp {

namespace {

BiquadCoefficients normalise(double b0, double b1, double b2, double a0, double a1, double a2)
{
    return { b0 / a0, b1 / a0, b2 / a0, a1 / a0, a2 / a0 };
}

} // namespace

BiquadCoefficients designBiquad(const FilterSpec& spec, double sampleRate)
{
    const auto nyquist = sampleRate * 0.5;
    const auto frequency = std::clamp(spec.frequencyHz, 10.0, nyquist * 0.98);
    const auto q = std::max(spec.q, 0.05);
    const auto omega = 2.0 * std::numbers::pi * frequency / sampleRate;
    const auto sinw = std::sin(omega);
    const auto cosw = std::cos(omega);
    const auto alpha = sinw / (2.0 * q);
    const auto a = std::pow(10.0, spec.gainDb / 40.0);

    switch (spec.type) {
        case FilterType::peaking:
            return normalise(1.0 + alpha * a,
                             -2.0 * cosw,
                             1.0 - alpha * a,
                             1.0 + alpha / a,
                             -2.0 * cosw,
                             1.0 - alpha / a);

        case FilterType::lowShelf: {
            const auto sqrtA = std::sqrt(a);
            const auto beta = 2.0 * sqrtA * alpha;
            return normalise(a * ((a + 1.0) - (a - 1.0) * cosw + beta),
                             2.0 * a * ((a - 1.0) - (a + 1.0) * cosw),
                             a * ((a + 1.0) - (a - 1.0) * cosw - beta),
                             (a + 1.0) + (a - 1.0) * cosw + beta,
                             -2.0 * ((a - 1.0) + (a + 1.0) * cosw),
                             (a + 1.0) + (a - 1.0) * cosw - beta);
        }

        case FilterType::highShelf: {
            const auto sqrtA = std::sqrt(a);
            const auto beta = 2.0 * sqrtA * alpha;
            return normalise(a * ((a + 1.0) + (a - 1.0) * cosw + beta),
                             -2.0 * a * ((a - 1.0) + (a + 1.0) * cosw),
                             a * ((a + 1.0) + (a - 1.0) * cosw - beta),
                             (a + 1.0) - (a - 1.0) * cosw + beta,
                             2.0 * ((a - 1.0) - (a + 1.0) * cosw),
                             (a + 1.0) - (a - 1.0) * cosw - beta);
        }

        case FilterType::lowPass:
            return normalise((1.0 - cosw) * 0.5,
                             1.0 - cosw,
                             (1.0 - cosw) * 0.5,
                             1.0 + alpha,
                             -2.0 * cosw,
                             1.0 - alpha);

        case FilterType::highPass:
            return normalise((1.0 + cosw) * 0.5,
                             -(1.0 + cosw),
                             (1.0 + cosw) * 0.5,
                             1.0 + alpha,
                             -2.0 * cosw,
                             1.0 - alpha);
    }

    return {};
}

} // namespace openref::dsp
