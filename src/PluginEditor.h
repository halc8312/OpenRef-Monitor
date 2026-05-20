#pragma once

#include "PluginProcessor.h"
#include "ui/MeterView.h"
#include "ui/ProfileSelector.h"
#include "ui/ResponseGraph.h"

#include <juce_gui_extra/juce_gui_extra.h>

class OpenRefMonitorAudioProcessorEditor final : public juce::AudioProcessorEditor, private juce::Timer {
public:
    explicit OpenRefMonitorAudioProcessorEditor(OpenRefMonitorAudioProcessor&);
    ~OpenRefMonitorAudioProcessorEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    OpenRefMonitorAudioProcessor& processor;
    openref::ui::ResponseGraph responseGraph;
    openref::ui::MeterView inputMeter { "Input" };
    openref::ui::MeterView outputMeter { "Output" };
    juce::Label title;
    juce::Label warning;
    juce::TextButton correctionButton { "Correction" };
    juce::ComboBox profileBox;
    juce::ComboBox targetBox;
    juce::ComboBox modeBox;
    juce::ComboBox translationBox;
    juce::Slider amountSlider;
    juce::Slider dryWetSlider;
    juce::Slider outputGainSlider;
    juce::Slider bassTiltSlider;
    juce::Slider trebleTiltSlider;
    juce::Label amountLabel;
    juce::Label dryWetLabel;
    juce::Label outputGainLabel;
    juce::Label bassTiltLabel;
    juce::Label trebleTiltLabel;
    juce::ToggleButton safeHeadroomButton { "Safe Headroom" };
    juce::ToggleButton autoBypassButton { "Auto-bypass offline render" };
    juce::ToggleButton monoButton { "Mono Check" };
    juce::Label latencyLabel;
    juce::Label attributionLabel;

    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    std::unique_ptr<ButtonAttachment> correctionAttachment;
    std::unique_ptr<ComboAttachment> profileAttachment;
    std::unique_ptr<ComboAttachment> targetAttachment;
    std::unique_ptr<ComboAttachment> modeAttachment;
    std::unique_ptr<ComboAttachment> translationAttachment;
    std::unique_ptr<SliderAttachment> amountAttachment;
    std::unique_ptr<SliderAttachment> dryWetAttachment;
    std::unique_ptr<SliderAttachment> outputGainAttachment;
    std::unique_ptr<SliderAttachment> bassTiltAttachment;
    std::unique_ptr<SliderAttachment> trebleTiltAttachment;
    std::unique_ptr<ButtonAttachment> safeHeadroomAttachment;
    std::unique_ptr<ButtonAttachment> autoBypassAttachment;
    std::unique_ptr<ButtonAttachment> monoAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OpenRefMonitorAudioProcessorEditor)
};
