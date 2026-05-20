#include "Parameters.h"

namespace openref {

juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back(std::make_unique<juce::AudioParameterBool>(ParameterIds::correctionEnabled, "Correction", true));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(ParameterIds::profileId, "Profile", juce::StringArray { "ATH-M70x AutoEq" }, 0));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(ParameterIds::targetId, "Target", juce::StringArray { "Neutral Harman-like", "Flat Custom" }, 0));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(ParameterIds::phaseMode, "Mode", juce::StringArray { "Zero Latency", "Minimum Phase", "Linear Phase" }, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterIds::amount, "Amount", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f }, 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterIds::dryWet, "Dry/Wet", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f }, 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterBool>(ParameterIds::safeHeadroom, "Safe Headroom", true));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterIds::outputGainDb, "Output Gain", juce::NormalisableRange<float> { -24.0f, 12.0f, 0.01f }, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterIds::bassTiltDb, "Bass Tilt", juce::NormalisableRange<float> { -6.0f, 6.0f, 0.01f }, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterIds::trebleTiltDb, "Treble Tilt", juce::NormalisableRange<float> { -6.0f, 6.0f, 0.01f }, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterBool>(ParameterIds::monoCheck, "Mono Check", false));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(ParameterIds::translationSimulationId, "Translation Check", juce::StringArray { "Off", "Small Phone Speaker", "Laptop Speaker", "Basic Car Check", "Mono Midrange", "Bass-limited Check", "Bright Earbuds Check" }, 0));
    params.push_back(std::make_unique<juce::AudioParameterBool>(ParameterIds::autoBypassOffline, "Auto-bypass Offline Render", true));
    params.push_back(std::make_unique<juce::AudioParameterBool>(ParameterIds::showRaw, "Show Raw", true));
    params.push_back(std::make_unique<juce::AudioParameterBool>(ParameterIds::showTarget, "Show Target", true));
    params.push_back(std::make_unique<juce::AudioParameterBool>(ParameterIds::showCorrection, "Show Correction", true));
    params.push_back(std::make_unique<juce::AudioParameterBool>(ParameterIds::showResult, "Show Result", true));
    return { params.begin(), params.end() };
}

} // namespace openref
