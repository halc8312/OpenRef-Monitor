#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace openref::ui {

class CurveEditor final : public juce::Component {
public:
    void paint(juce::Graphics& g) override;
};

} // namespace openref::ui
