#include "ResponseGraph.h"

#include <cmath>

namespace openref::ui {

void ResponseGraph::setProfile(const profile::CalibrationProfile& newProfile)
{
    profile = newProfile;
    repaint();
}

juce::Path ResponseGraph::makePath(const std::vector<dsp::MagnitudePoint>& points, juce::Rectangle<float> bounds) const
{
    juce::Path path;
    auto first = true;
    for (const auto& point : points) {
        const auto xNorm = static_cast<float>((std::log(point.hz) - std::log(20.0)) / (std::log(20000.0) - std::log(20.0)));
        const auto yNorm = static_cast<float>((point.db + 18.0) / 36.0);
        const auto x = bounds.getX() + xNorm * bounds.getWidth();
        const auto y = bounds.getBottom() - yNorm * bounds.getHeight();
        if (first) {
            path.startNewSubPath(x, y);
            first = false;
        } else {
            path.lineTo(x, y);
        }
    }
    return path;
}

void ResponseGraph::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(12.0f);
    g.setColour(juce::Colour(0xff0b0e12));
    g.fillRoundedRectangle(bounds, 10.0f);

    g.setColour(juce::Colour(0xff334050));
    for (auto hz : { 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000 }) {
        const auto xNorm = static_cast<float>((std::log(static_cast<double>(hz)) - std::log(20.0)) / (std::log(20000.0) - std::log(20.0)));
        const auto x = bounds.getX() + xNorm * bounds.getWidth();
        g.drawVerticalLine(static_cast<int>(x), bounds.getY(), bounds.getBottom());
    }
    for (auto db : { -18, -12, -6, 0, 6, 12, 18 }) {
        const auto yNorm = static_cast<float>((db + 18.0) / 36.0);
        const auto y = bounds.getBottom() - yNorm * bounds.getHeight();
        g.drawHorizontalLine(static_cast<int>(y), bounds.getX(), bounds.getRight());
    }

    g.setColour(juce::Colours::skyblue);
    g.strokePath(makePath(profile.left.frequencyResponse, bounds), juce::PathStrokeType(1.6f));
    g.setColour(juce::Colours::orange);
    g.strokePath(makePath(profile.left.correctionCurve, bounds), juce::PathStrokeType(2.0f));
    g.setColour(juce::Colours::white);
    g.drawText("Raw / Correction / Target graph", bounds.toNearestInt().reduced(10), juce::Justification::topLeft);
}

} // namespace openref::ui
