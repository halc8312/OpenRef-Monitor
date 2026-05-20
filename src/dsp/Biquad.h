#pragma once

#include <array>
#include <complex>

namespace openref::dsp {

enum class FilterType {
    peaking,
    lowShelf,
    highShelf,
    lowPass,
    highPass
};

struct BiquadCoefficients {
    double b0 { 1.0 };
    double b1 { 0.0 };
    double b2 { 0.0 };
    double a1 { 0.0 };
    double a2 { 0.0 };
};

class Biquad {
public:
    void setCoefficients(const BiquadCoefficients& newCoefficients) noexcept;
    void reset() noexcept;
    float processSample(float input) noexcept;
    [[nodiscard]] double magnitudeDb(double frequencyHz, double sampleRate) const noexcept;
    [[nodiscard]] const BiquadCoefficients& coefficients() const noexcept { return coeffs; }

private:
    BiquadCoefficients coeffs {};
    double z1 { 0.0 };
    double z2 { 0.0 };
};

} // namespace openref::dsp
