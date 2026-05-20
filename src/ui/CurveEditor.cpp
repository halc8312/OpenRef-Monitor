#include "CurveEditor.h"

namespace openref::ui {

void CurveEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff151a20));
    g.setColour(juce::Colours::white);
    g.drawFittedText("Custom target curve editor (v1.5)", getLocalBounds(), juce::Justification::centred, 1);
}

} // namespace openref::ui
