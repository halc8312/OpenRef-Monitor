#include "PluginProcessor.h"

#include "PluginEditor.h"
#include "profile/ProfileJson.h"
#include "dsp/ResponseAnalyzer.h"

#include <BinaryData.h>

#include <algorithm>
#include <array>
#include <cmath>

OpenRefMonitorAudioProcessor::OpenRefMonitorAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "PARAMETERS", openref::createParameterLayout())
{
    loadBundledProfile();
    loadBundledTargetsAndSimulations();
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
    updateExtraFilters();
    const auto translationIndex = static_cast<int>(parameters.getRawParameterValue(openref::ParameterIds::translationSimulationId)->load());
    const auto simulationMono = translationIndex > 0
        && static_cast<std::size_t>(translationIndex - 1) < simulations.size()
        && simulations[static_cast<std::size_t>(translationIndex - 1)].mono;
    const auto monoCheck = parameters.getRawParameterValue(openref::ParameterIds::monoCheck)->load() > 0.5f || simulationMono;
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

void OpenRefMonitorAudioProcessor::loadBundledTargetsAndSimulations()
{
    auto parseTarget = [](const char* data, int size) {
        const auto text = juce::String::fromUTF8(data, size).toStdString();
        return openref::profile::parseTargetCurve(nlohmann::json::parse(text));
    };
    auto parseSimulation = [](const char* data, int size) {
        const auto text = juce::String::fromUTF8(data, size).toStdString();
        return openref::profile::parseSimulationProfile(nlohmann::json::parse(text));
    };

    targets = {
        parseTarget(BinaryData::neutral_harman_like_json, BinaryData::neutral_harman_like_jsonSize),
        parseTarget(BinaryData::flat_custom_json, BinaryData::flat_custom_jsonSize),
    };
    simulations = {
        parseSimulation(BinaryData::phone_small_speaker_json, BinaryData::phone_small_speaker_jsonSize),
        parseSimulation(BinaryData::laptop_speaker_json, BinaryData::laptop_speaker_jsonSize),
        parseSimulation(BinaryData::car_basic_json, BinaryData::car_basic_jsonSize),
        parseSimulation(BinaryData::mono_midrange_json, BinaryData::mono_midrange_jsonSize),
        parseSimulation(BinaryData::bass_limited_check_json, BinaryData::bass_limited_check_jsonSize),
        parseSimulation(BinaryData::bright_earbuds_check_json, BinaryData::bright_earbuds_check_jsonSize),
    };
}

void OpenRefMonitorAudioProcessor::rebuildEngine(double sampleRate)
{
    engine.prepare(sampleRate, profile.left.parametricEq, profile.right.parametricEq);
    currentAutoPreampDb = openref::dsp::calculateSafeHeadroomDb(profile.left.parametricEq, sampleRate);
    lastTargetIndex = -1;
    lastTranslationIndex = -1;
    updateExtraFilters();
}

void OpenRefMonitorAudioProcessor::appendTiltFilters(std::array<openref::dsp::FilterSpec, maxRealtimeExtraFilters>& filters,
                                                     std::size_t& filterCount,
                                                     float bassTiltDb,
                                                     float trebleTiltDb)
{
    if (std::abs(bassTiltDb) > 0.001f && filterCount < filters.size()) {
        filters[filterCount++] = { openref::dsp::FilterType::lowShelf, 120.0, bassTiltDb, 0.7 };
    }
    if (std::abs(trebleTiltDb) > 0.001f && filterCount < filters.size()) {
        filters[filterCount++] = { openref::dsp::FilterType::highShelf, 8000.0, trebleTiltDb, 0.7 };
    }
}

void OpenRefMonitorAudioProcessor::updateExtraFilters()
{
    const auto targetIndex = static_cast<int>(parameters.getRawParameterValue(openref::ParameterIds::targetId)->load());
    const auto translationIndex = static_cast<int>(parameters.getRawParameterValue(openref::ParameterIds::translationSimulationId)->load());
    const auto bassTilt = parameters.getRawParameterValue(openref::ParameterIds::bassTiltDb)->load();
    const auto trebleTilt = parameters.getRawParameterValue(openref::ParameterIds::trebleTiltDb)->load();

    if (targetIndex == lastTargetIndex
        && translationIndex == lastTranslationIndex
        && std::abs(bassTilt - lastBassTiltDb) < 0.001f
        && std::abs(trebleTilt - lastTrebleTiltDb) < 0.001f) {
        return;
    }

    std::array<openref::dsp::FilterSpec, maxRealtimeExtraFilters> filters {};
    std::size_t filterCount {};

    if (targetIndex == 0) {
        filters[filterCount++] = { openref::dsp::FilterType::lowShelf, 105.0, 1.5, 0.7 };
        filters[filterCount++] = { openref::dsp::FilterType::highShelf, 10000.0, -1.0, 0.7 };
    }

    appendTiltFilters(filters, filterCount, bassTilt, trebleTilt);

    if (translationIndex > 0) {
        const auto simulationIndex = static_cast<std::size_t>(translationIndex - 1);
        if (simulationIndex < simulations.size()) {
            const auto remaining = maxRealtimeExtraFilters - filterCount;
            const auto copyCount = std::min(remaining, simulations[simulationIndex].filters.size());
            for (std::size_t i = 0; i < copyCount; ++i) {
                filters[filterCount++] = simulations[simulationIndex].filters[i];
            }
        }
    }

    std::copy_n(filters.begin(), filterCount, realtimeExtraFilters.begin());
    engine.setExtraFilters(realtimeExtraFilters.data(), filterCount);
    currentAutoPreampDb = calculateCurrentSafeHeadroomDb();

    lastTargetIndex = targetIndex;
    lastTranslationIndex = translationIndex;
    lastBassTiltDb = bassTilt;
    lastTrebleTiltDb = trebleTilt;
}

double OpenRefMonitorAudioProcessor::calculateCurrentSafeHeadroomDb() const noexcept
{
    double maxBoost = 0.0;
    constexpr int points = 512;
    for (int i = 0; i < points; ++i) {
        const auto t = static_cast<double>(i) / (points - 1);
        const auto hz = std::exp(std::log(20.0) + t * (std::log(20000.0) - std::log(20.0)));
        maxBoost = std::max(maxBoost, engine.responseDb(hz, 0));
    }
    return -std::max(0.0, maxBoost) - 1.0;
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OpenRefMonitorAudioProcessor();
}
