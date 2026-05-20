#include "FirDesigner.h"

#include <algorithm>
#include <cmath>
#include <numbers>

namespace openref::dsp {

namespace {

double interpolateDb(const std::vector<MagnitudePoint>& curve, double hz)
{
    if (curve.empty()) {
        return 0.0;
    }
    if (hz <= curve.front().hz) {
        return curve.front().db;
    }
    if (hz >= curve.back().hz) {
        return curve.back().db;
    }

    const auto logHz = std::log(hz);
    for (std::size_t i = 1; i < curve.size(); ++i) {
        if (hz <= curve[i].hz) {
            const auto lo = std::log(curve[i - 1].hz);
            const auto hi = std::log(curve[i].hz);
            const auto t = (logHz - lo) / (hi - lo);
            return curve[i - 1].db + t * (curve[i].db - curve[i - 1].db);
        }
    }
    return curve.back().db;
}

} // namespace

std::vector<float> designLinearPhaseFir(const std::vector<MagnitudePoint>& curve, double sampleRate, int length)
{
    const auto n = std::max(64, length);
    std::vector<float> ir(static_cast<std::size_t>(n), 0.0f);
    const auto centre = (n - 1) * 0.5;

    for (int i = 0; i < n; ++i) {
        const auto x = static_cast<double>(i) - centre;
        const auto window = 0.5 - 0.5 * std::cos(2.0 * std::numbers::pi * i / (n - 1));
        auto acc = 0.0;
        constexpr int bins = 1024;
        for (int k = 1; k < bins; ++k) {
            const auto hz = (sampleRate * 0.5) * k / bins;
            const auto clampedDb = std::clamp(interpolateDb(curve, hz), -12.0, 6.0);
            const auto mag = std::pow(10.0, clampedDb / 20.0);
            acc += mag * std::cos(2.0 * std::numbers::pi * hz * x / sampleRate);
        }
        ir[static_cast<std::size_t>(i)] = static_cast<float>((acc / bins) * window);
    }
    return ir;
}

std::vector<float> designMinimumPhaseFir(const std::vector<MagnitudePoint>& curve, double sampleRate, int length)
{
    auto ir = designLinearPhaseFir(curve, sampleRate, length);
    if (! ir.empty()) {
        std::rotate(ir.begin(), ir.begin() + static_cast<std::ptrdiff_t>(ir.size() / 2), ir.end());
    }
    return ir;
}

} // namespace openref::dsp
