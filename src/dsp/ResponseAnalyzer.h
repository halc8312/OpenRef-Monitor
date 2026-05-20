#pragma once

#include "BiquadDesigner.h"
#include "FirDesigner.h"

#include <vector>

namespace openref::dsp {

double calculateSafeHeadroomDb(const std::vector<FilterSpec>& filters, double sampleRate);
double maxCurveBoostDb(const std::vector<MagnitudePoint>& curve);

} // namespace openref::dsp
