#pragma once

#include <vector>

namespace openref::dsp {

struct MagnitudePoint {
    double hz {};
    double db {};
};

std::vector<float> designLinearPhaseFir(const std::vector<MagnitudePoint>& curve, double sampleRate, int length);
std::vector<float> designMinimumPhaseFir(const std::vector<MagnitudePoint>& curve, double sampleRate, int length);

} // namespace openref::dsp
