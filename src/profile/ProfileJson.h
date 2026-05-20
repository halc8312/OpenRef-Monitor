#pragma once

#include "CalibrationProfile.h"

#include <nlohmann/json.hpp>

namespace openref::profile {

CalibrationProfile parseCalibrationProfile(const nlohmann::json& json);
TargetCurve parseTargetCurve(const nlohmann::json& json);
SimulationProfile parseSimulationProfile(const nlohmann::json& json);
nlohmann::json toJson(const CalibrationProfile& profile);

} // namespace openref::profile
