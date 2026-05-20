#include "ResponseAnalyzer.h"

#include "Biquad.h"

#include <algorithm>
#include <cmath>

namespace openref::dsp {

double calculateSafeHeadroomDb(const std::vector<FilterSpec>& filters, double sampleRate)
{
    std::vector<Biquad> biquads;
    biquads.reserve(filters.size());
    for (const auto& spec : filters) {
        Biquad biquad;
        biquad.setCoefficients(designBiquad(spec, sampleRate));
        biquads.push_back(biquad);
    }

    double maxBoost = 0.0;
    constexpr int points = 512;
    for (int i = 0; i < points; ++i) {
        const auto t = static_cast<double>(i) / (points - 1);
        const auto hz = std::exp(std::log(20.0) + t * (std::log(20000.0) - std::log(20.0)));
        auto db = 0.0;
        for (const auto& filter : biquads) {
            db += filter.magnitudeDb(hz, sampleRate);
        }
        maxBoost = std::max(maxBoost, db);
    }

    return -std::max(0.0, maxBoost) - 1.0;
}

double maxCurveBoostDb(const std::vector<MagnitudePoint>& curve)
{
    double maxBoost = 0.0;
    for (const auto& point : curve) {
        maxBoost = std::max(maxBoost, point.db);
    }
    return maxBoost;
}

} // namespace openref::dsp
