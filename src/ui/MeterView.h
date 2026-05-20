#pragma once

#include "dsp/Metering.h"

#include <juce_gui_basics/juce_gui_basics.h>

namespace openref::ui {

class MeterView final : public juce::Component {
public:
    explicit MeterView(juce::String labelText);
    void setMeterState(dsp::MeterState state);
    void paint(juce::Graphics& g) override;

private:
    juce::String label;
    dsp::MeterState meter;
};

} // namespace openref::ui
