#pragma once

#include "Biquad.h"
#include "BiquadDesigner.h"

#include <array>
#include <cstddef>
#include <vector>

namespace openref::dsp {

class CorrectionEngine {
public:
    static constexpr std::size_t maxExtraFilters = 16;

    void prepare(double newSampleRate, const std::vector<FilterSpec>& leftSpecs, const std::vector<FilterSpec>& rightSpecs);
    void reset() noexcept;
    void setOutputGainDb(double gainDb) noexcept;
    void setExtraFilters(const FilterSpec* specs, std::size_t count);
    void process(float* left, float* right, std::size_t count, float dryWet, bool monoCheck) noexcept;
    [[nodiscard]] double responseDb(double frequencyHz, int channel) const noexcept;
    [[nodiscard]] double sampleRate() const noexcept { return currentSampleRate; }

private:
    std::vector<Biquad> leftFilters;
    std::vector<Biquad> rightFilters;
    std::array<Biquad, maxExtraFilters> leftExtraFilters;
    std::array<Biquad, maxExtraFilters> rightExtraFilters;
    std::size_t extraFilterCount {};
    double currentSampleRate { 48000.0 };
    float outputGainLinear { 1.0f };
};

} // namespace openref::dsp
