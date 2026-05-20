#pragma once

#include "dsp/BiquadDesigner.h"
#include "dsp/FirDesigner.h"

#include <string>
#include <vector>

namespace openref::profile {

struct ProfileSource {
    std::string name;
    std::string url;
    std::string license;
    std::string measurementSource;
    std::string generatedAt;
};

struct ProfileChannel {
    std::vector<dsp::MagnitudePoint> frequencyResponse;
    std::vector<dsp::MagnitudePoint> correctionCurve;
    std::vector<dsp::FilterSpec> parametricEq;
    std::string sameAs;
};

struct CalibrationProfile {
    int schemaVersion { 1 };
    std::string id;
    std::string manufacturer;
    std::string model;
    std::string type;
    ProfileSource source;
    double recommendedPreampDb {};
    ProfileChannel left;
    ProfileChannel right;
};

struct TargetCurve {
    std::string id;
    std::string displayName;
    std::vector<dsp::MagnitudePoint> points;
};

struct SimulationProfile {
    std::string id;
    std::string displayName;
    std::string description;
    std::vector<dsp::FilterSpec> filters;
    bool mono {};
};

} // namespace openref::profile
