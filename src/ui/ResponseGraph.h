#pragma once

#include "profile/CalibrationProfile.h"

#include <juce_gui_basics/juce_gui_basics.h>

namespace openref::ui {

class ResponseGraph final : public juce::Component {
public:
    void setProfile(const profile::CalibrationProfile& newProfile);
    void paint(juce::Graphics& g) override;

private:
    juce::Path makePath(const std::vector<dsp::MagnitudePoint>& points, juce::Rectangle<float> bounds) const;
    profile::CalibrationProfile profile;
};

} // namespace openref::ui
