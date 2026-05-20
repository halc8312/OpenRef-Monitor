#include "PluginProcessor.h"

#include "PluginEditor.h"
#include "profile/ProfileJson.h"
#include "dsp/ResponseAnalyzer.h"

#include <BinaryData.h>

OpenRefMonitorAudioProcessor::OpenRefMonitorAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "PARAMETERS", openref::createParameterLayout())
{
    loadBundledProfile();
}

void OpenRefMonitorAudioProcessor::prepareToPlay(double sampleRate, int)
{
    correctionRamp.prepare(sampleRate);
    monoScratch.assign(32768, 0.0f);
    rebuildEngine(sampleRate);
}

bool OpenRefMonitorAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto& input = layouts.getMainInputChannelSet();
    const auto& output = layouts.getMainOutputChannelSet();
    return input == output && (output == juce::AudioChannelSet::mono() || output == juce::AudioChannelSet::stereo());
}

void OpenRefMonitorAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);
    juce::ScopedNoDenormals noDenormals;

    const auto totalIn = getTotalNumInputChannels();
    const auto totalOut = getTotalNumOutputChannels();
    for (auto channel = totalIn; channel < totalOut; ++channel) {
        buffer.clear(channel, 0, buffer.getNumSamples());
    }

    if (buffer.getNumChannels() == 0 || buffer.getNumSamples() == 0) {
        return;
    }

    inputMeter.process(buffer.getReadPointer(0), static_cast<std::size_t>(buffer.getNumSamples()));
    lastInputMeter = inputMeter.state();

    const auto enabled = parameters.getRawParameterValue(openref::ParameterIds::correctionEnabled)->load() > 0.5f;
    const auto autoBypassOffline = parameters.getRawParameterValue(openref::ParameterIds::autoBypassOffline)->load() > 0.5f;
    const auto bypassForRender = autoBypassOffline && isNonRealtime();
    correctionRamp.setEnabled(enabled && ! bypassForRender);

    const auto dryWet = parameters.getRawParameterValue(openref::ParameterIds::dryWet)->load();
    const auto amount = parameters.getRawParameterValue(openref::ParameterIds::amount)->load();
    const auto outputGain = parameters.getRawParameterValue(openref::ParameterIds::outputGainDb)->load();
    const auto safeHeadroom = parameters.getRawParameterValue(openref::ParameterIds::safeHeadroom)->load() > 0.5f;
    const auto monoCheck = parameters.getRawParameterValue(openref::ParameterIds::monoCheck)->load() > 0.5f;
    engine.setOutputGainDb(outputGain + (safeHeadroom ? currentAutoPreampDb : 0.0));

    if (buffer.getNumChannels() == 1) {
        auto* mono = buffer.getWritePointer(0);
        const auto count = static_cast<std::size_t>(std::min(buffer.getNumSamples(), static_cast<int>(monoScratch.size())));
        std::copy(mono, mono + count, monoScratch.begin());
        for (std::size_t i = 0; i < count; ++i) {
            const auto correctionMix = dryWet * amount * correctionRamp.next();
            engine.process(mono + i, monoScratch.data() + i, 1, correctionMix, false);
        }
    } else {
        auto* left = buffer.getWritePointer(0);
        auto* right = buffer.getWritePointer(1);
        for (int i = 0; i < buffer.getNumSamples(); ++i) {
            const auto correctionMix = dryWet * amount * correctionRamp.next();
            engine.process(left + i, right + i, 1, correctionMix, monoCheck);
        }
    }

    outputMeter.process(buffer.getReadPointer(0), static_cast<std::size_t>(buffer.getNumSamples()));
    lastOutputMeter = outputMeter.state();
}

juce::AudioProcessorEditor* OpenRefMonitorAudioProcessor::createEditor()
{
    return new OpenRefMonitorAudioProcessorEditor(*this);
}

void OpenRefMonitorAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    if (auto xml = parameters.copyState().createXml()) {
        copyXmlToBinary(*xml, destData);
    }
}

void OpenRefMonitorAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes)) {
        parameters.replaceState(juce::ValueTree::fromXml(*xml));
    }
}

void OpenRefMonitorAudioProcessor::loadBundledProfile()
{
    const auto data = juce::String::fromUTF8(BinaryData::audiotechnicaathm70x_autoeq_json,
                                            BinaryData::audiotechnicaathm70x_autoeq_jsonSize);
    profile = openref::profile::parseCalibrationProfile(nlohmann::json::parse(data.toStdString()));
}

void OpenRefMonitorAudioProcessor::rebuildEngine(double sampleRate)
{
    engine.prepare(sampleRate, profile.left.parametricEq, profile.right.parametricEq);
    currentAutoPreampDb = openref::dsp::calculateSafeHeadroomDb(profile.left.parametricEq, sampleRate);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OpenRefMonitorAudioProcessor();
}
