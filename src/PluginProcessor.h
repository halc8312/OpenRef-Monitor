#pragma once

#include "Parameters.h"
#include "dsp/CorrectionEngine.h"
#include "dsp/Metering.h"
#include "dsp/SmoothedBypass.h"
#include "profile/CalibrationProfile.h"

#include <juce_audio_processors/juce_audio_processors.h>

class OpenRefMonitorAudioProcessor final : public juce::AudioProcessor {
public:
    OpenRefMonitorAudioProcessor();
    ~OpenRefMonitorAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "OpenRef Monitor"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState parameters;
    [[nodiscard]] openref::dsp::MeterState inputMeterState() const noexcept { return lastInputMeter; }
    [[nodiscard]] openref::dsp::MeterState outputMeterState() const noexcept { return lastOutputMeter; }
    [[nodiscard]] double autoPreampDb() const noexcept { return currentAutoPreampDb; }
    [[nodiscard]] const openref::profile::CalibrationProfile& activeProfile() const noexcept { return profile; }

private:
    void loadBundledProfile();
    void rebuildEngine(double sampleRate);

    openref::profile::CalibrationProfile profile;
    openref::dsp::CorrectionEngine engine;
    openref::dsp::PeakMeter inputMeter;
    openref::dsp::PeakMeter outputMeter;
    openref::dsp::SmoothedBypass correctionRamp;
    openref::dsp::MeterState lastInputMeter;
    openref::dsp::MeterState lastOutputMeter;
    std::vector<float> monoScratch;
    double currentAutoPreampDb {};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OpenRefMonitorAudioProcessor)
};
