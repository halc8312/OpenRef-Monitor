#include "ProfileSelector.h"

namespace openref::ui {

ProfileSelector::ProfileSelector()
{
    addItem("Audio-Technica ATH-M70x (AutoEq)", 1);
    setSelectedId(1, juce::dontSendNotification);
}

} // namespace openref::ui
