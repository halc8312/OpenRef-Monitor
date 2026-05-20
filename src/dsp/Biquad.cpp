#include "Biquad.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <numbers>

namespace openref::dsp {

void Biquad::setCoefficients(const BiquadCoefficients& newCoefficients) noexcept
{
    coeffs = newCoefficients;
}

void Biquad::reset() noexcept
{
    z1 = 0.0;
    z2 = 0.0;
}

float Biquad::processSample(float input) noexcept
{
    if (! std::isfinite(input)) {
        input = 0.0f;
    }

    const auto output = coeffs.b0 * input + z1;
    z1 = coeffs.b1 * input - coeffs.a1 * output + z2;
    z2 = coeffs.b2 * input - coeffs.a2 * output;

    if (! std::isfinite(output)) {
        reset();
        return 0.0f;
    }

    return static_cast<float>(std::clamp(output, -32.0, 32.0));
}

double Biquad::magnitudeDb(double frequencyHz, double sampleRate) const noexcept
{
    if (frequencyHz <= 0.0 || sampleRate <= 0.0) {
        return 0.0;
    }

    const auto omega = 2.0 * std::numbers::pi * frequencyHz / sampleRate;
    const std::complex<double> z1v = std::exp(std::complex<double>(0.0, -omega));
    const auto z2v = z1v * z1v;
    const auto numerator = coeffs.b0 + coeffs.b1 * z1v + coeffs.b2 * z2v;
    const auto denominator = 1.0 + coeffs.a1 * z1v + coeffs.a2 * z2v;
    const auto magnitude = std::abs(numerator / denominator);
    return 20.0 * std::log10(std::max(magnitude, std::numeric_limits<double>::min()));
}

} // namespace openref::dsp
