#include "CorrectionEngine.h"

#include <algorithm>
#include <cmath>

namespace openref::dsp {

void CorrectionEngine::prepare(double newSampleRate, const std::vector<FilterSpec>& leftSpecs, const std::vector<FilterSpec>& rightSpecs)
{
    currentSampleRate = newSampleRate > 0.0 ? newSampleRate : 48000.0;
    leftFilters.clear();
    rightFilters.clear();
    leftFilters.reserve(leftSpecs.size());
    rightFilters.reserve(rightSpecs.empty() ? leftSpecs.size() : rightSpecs.size());

    for (const auto& spec : leftSpecs) {
        Biquad filter;
        filter.setCoefficients(designBiquad(spec, currentSampleRate));
        leftFilters.push_back(filter);
    }

    const auto& sourceRight = rightSpecs.empty() ? leftSpecs : rightSpecs;
    for (const auto& spec : sourceRight) {
        Biquad filter;
        filter.setCoefficients(designBiquad(spec, currentSampleRate));
        rightFilters.push_back(filter);
    }
}

void CorrectionEngine::reset() noexcept
{
    for (auto& filter : leftFilters) {
        filter.reset();
    }
    for (auto& filter : rightFilters) {
        filter.reset();
    }
}

void CorrectionEngine::setOutputGainDb(double gainDb) noexcept
{
    outputGainLinear = static_cast<float>(std::pow(10.0, gainDb / 20.0));
}

void CorrectionEngine::process(float* left, float* right, std::size_t count, float dryWet, bool monoCheck) noexcept
{
    const auto wet = std::clamp(dryWet, 0.0f, 1.0f);
    const auto dry = 1.0f - wet;

    for (std::size_t i = 0; i < count; ++i) {
        const auto dryLeft = left[i];
        const auto dryRight = right[i];
        auto wetLeft = dryLeft;
        auto wetRight = dryRight;

        for (auto& filter : leftFilters) {
            wetLeft = filter.processSample(wetLeft);
        }
        for (auto& filter : rightFilters) {
            wetRight = filter.processSample(wetRight);
        }

        if (monoCheck) {
            const auto mono = 0.5f * (wetLeft + wetRight);
            wetLeft = mono;
            wetRight = mono;
        }

        left[i] = (dryLeft * dry + wetLeft * wet) * outputGainLinear;
        right[i] = (dryRight * dry + wetRight * wet) * outputGainLinear;
    }
}

double CorrectionEngine::responseDb(double frequencyHz, int channel) const noexcept
{
    const auto& filters = channel == 1 ? rightFilters : leftFilters;
    double total = 0.0;
    for (const auto& filter : filters) {
        total += filter.magnitudeDb(frequencyHz, currentSampleRate);
    }
    return total;
}

} // namespace openref::dsp
