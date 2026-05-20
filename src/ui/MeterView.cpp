#include "MeterView.h"

namespace openref::ui {

MeterView::MeterView(juce::String labelText) : label(std::move(labelText)) {}

void MeterView::setMeterState(dsp::MeterState state)
{
    meter = state;
    repaint();
}

void MeterView::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.setColour(juce::Colour(0xff0b0e12));
    g.fillRoundedRectangle(bounds, 6.0f);
    const auto fill = bounds.removeFromBottom(bounds.getHeight() * juce::jlimit(0.0f, 1.0f, meter.peak));
    g.setColour(meter.clipped ? juce::Colours::red : juce::Colours::limegreen);
    g.fillRoundedRectangle(fill.reduced(3.0f), 4.0f);
    g.setColour(juce::Colours::white);
    g.drawFittedText(label, getLocalBounds().reduced(4), juce::Justification::centredBottom, 1);
}

} // namespace openref::ui
