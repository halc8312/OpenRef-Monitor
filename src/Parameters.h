#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace openref {

struct ParameterIds {
    static constexpr auto correctionEnabled = "correctionEnabled";
    static constexpr auto profileId = "profileId";
    static constexpr auto targetId = "targetId";
    static constexpr auto phaseMode = "phaseMode";
    static constexpr auto amount = "amount";
    static constexpr auto dryWet = "dryWet";
    static constexpr auto safeHeadroom = "safeHeadroom";
    static constexpr auto outputGainDb = "outputGainDb";
    static constexpr auto bassTiltDb = "bassTiltDb";
    static constexpr auto trebleTiltDb = "trebleTiltDb";
    static constexpr auto monoCheck = "monoCheck";
    static constexpr auto translationSimulationId = "translationSimulationId";
    static constexpr auto autoBypassOffline = "autoBypassOffline";
    static constexpr auto showRaw = "showRaw";
    static constexpr auto showTarget = "showTarget";
    static constexpr auto showCorrection = "showCorrection";
    static constexpr auto showResult = "showResult";
};

juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

} // namespace openref
