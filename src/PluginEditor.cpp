#include "PluginEditor.h"

#include "Parameters.h"

OpenRefMonitorAudioProcessorEditor::OpenRefMonitorAudioProcessorEditor(OpenRefMonitorAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(1080, 650);

    title.setText("OpenRef Monitor", juce::dontSendNotification);
    title.setFont(juce::Font(juce::FontOptions(26.0f, juce::Font::bold)));
    addAndMakeVisible(title);

    warning.setText("Monitoring only - do not print correction to mix. Use Cubase Control Room Monitor Insert.", juce::dontSendNotification);
    warning.setColour(juce::Label::textColourId, juce::Colours::orange);
    addAndMakeVisible(warning);

    addAndMakeVisible(responseGraph);
    responseGraph.setProfile(processor.activeProfile());

    addAndMakeVisible(inputMeter);
    addAndMakeVisible(outputMeter);

    correctionButton.setClickingTogglesState(true);
    addAndMakeVisible(correctionButton);

    profileBox.addItemList({ "ATH-M70x AutoEq" }, 1);
    addAndMakeVisible(profileBox);

    targetBox.addItemList({ "Neutral Harman-like", "Flat Custom" }, 1);
    addAndMakeVisible(targetBox);

    modeBox.addItemList({ "Zero Latency", "Minimum Phase", "Linear Phase" }, 1);
    addAndMakeVisible(modeBox);

    translationBox.addItemList({ "Off", "Small Phone Speaker", "Laptop Speaker", "Basic Car Check", "Mono Midrange", "Bass-limited Check", "Bright Earbuds Check" }, 1);
    addAndMakeVisible(translationBox);

    for (auto* slider : { &amountSlider, &dryWetSlider, &outputGainSlider, &bassTiltSlider, &trebleTiltSlider }) {
        slider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 90, 22);
        addAndMakeVisible(slider);
    }
    auto configureSliderLabel = [this](juce::Label& label, juce::String text) {
        label.setText(text, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        label.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
        addAndMakeVisible(label);
    };
    configureSliderLabel(amountLabel, "Amount");
    configureSliderLabel(dryWetLabel, "Dry/Wet");
    configureSliderLabel(outputGainLabel, "Output");
    configureSliderLabel(bassTiltLabel, "Bass Tilt");
    configureSliderLabel(trebleTiltLabel, "Treble Tilt");

    addAndMakeVisible(safeHeadroomButton);
    addAndMakeVisible(autoBypassButton);
    addAndMakeVisible(monoButton);

    latencyLabel.setText("Latency: 0 samples / 0.0 ms", juce::dontSendNotification);
    addAndMakeVisible(latencyLabel);

    attributionLabel.setText("ATH-M70x profile: AutoEq MIT; measurement source attribution in About/README.", juce::dontSendNotification);
    attributionLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    addAndMakeVisible(attributionLabel);

    correctionAttachment = std::make_unique<ButtonAttachment>(processor.parameters, openref::ParameterIds::correctionEnabled, correctionButton);
    modeAttachment = std::make_unique<ComboAttachment>(processor.parameters, openref::ParameterIds::phaseMode, modeBox);
    translationAttachment = std::make_unique<ComboAttachment>(processor.parameters, openref::ParameterIds::translationSimulationId, translationBox);
    amountAttachment = std::make_unique<SliderAttachment>(processor.parameters, openref::ParameterIds::amount, amountSlider);
    dryWetAttachment = std::make_unique<SliderAttachment>(processor.parameters, openref::ParameterIds::dryWet, dryWetSlider);
    outputGainAttachment = std::make_unique<SliderAttachment>(processor.parameters, openref::ParameterIds::outputGainDb, outputGainSlider);
    safeHeadroomAttachment = std::make_unique<ButtonAttachment>(processor.parameters, openref::ParameterIds::safeHeadroom, safeHeadroomButton);
    autoBypassAttachment = std::make_unique<ButtonAttachment>(processor.parameters, openref::ParameterIds::autoBypassOffline, autoBypassButton);
    monoAttachment = std::make_unique<ButtonAttachment>(processor.parameters, openref::ParameterIds::monoCheck, monoButton);
    profileAttachment = std::make_unique<ComboAttachment>(processor.parameters, openref::ParameterIds::profileId, profileBox);
    targetAttachment = std::make_unique<ComboAttachment>(processor.parameters, openref::ParameterIds::targetId, targetBox);
    bassTiltAttachment = std::make_unique<SliderAttachment>(processor.parameters, openref::ParameterIds::bassTiltDb, bassTiltSlider);
    trebleTiltAttachment = std::make_unique<SliderAttachment>(processor.parameters, openref::ParameterIds::trebleTiltDb, trebleTiltSlider);

    startTimerHz(30);
}

void OpenRefMonitorAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff11161c));
    g.setColour(juce::Colour(0xff243140));
    g.fillRoundedRectangle(getLocalBounds().reduced(12).toFloat(), 12.0f);
}

void OpenRefMonitorAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(22);
    auto top = area.removeFromTop(62);
    title.setBounds(top.removeFromLeft(210));
    correctionButton.setBounds(top.removeFromLeft(105).reduced(4));
    profileBox.setBounds(top.removeFromLeft(155).reduced(4));
    targetBox.setBounds(top.removeFromLeft(150).reduced(4));
    modeBox.setBounds(top.removeFromLeft(135).reduced(4));
    autoBypassButton.setBounds(top.removeFromLeft(200).reduced(4));

    auto subTop = area.removeFromTop(48);
    translationBox.setBounds(subTop.removeFromLeft(260).reduced(4));

    warning.setBounds(area.removeFromBottom(32));
    attributionLabel.setBounds(area.removeFromBottom(26));

    auto right = area.removeFromRight(260).reduced(10);
    auto sliderGrid = right.removeFromTop(312);
    auto layoutSlider = [](juce::Rectangle<int> cell, juce::Label& label, juce::Slider& slider) {
        cell.reduce(4, 2);
        label.setBounds(cell.removeFromTop(18));
        slider.setBounds(cell);
    };
    auto firstRow = sliderGrid.removeFromTop(104);
    layoutSlider(firstRow.removeFromLeft(120), amountLabel, amountSlider);
    layoutSlider(firstRow.removeFromLeft(120), dryWetLabel, dryWetSlider);
    auto secondRow = sliderGrid.removeFromTop(104);
    layoutSlider(secondRow.removeFromLeft(120), outputGainLabel, outputGainSlider);
    layoutSlider(secondRow.removeFromLeft(120), bassTiltLabel, bassTiltSlider);
    auto thirdRow = sliderGrid.removeFromTop(104);
    layoutSlider(thirdRow.removeFromLeft(120), trebleTiltLabel, trebleTiltSlider);
    safeHeadroomButton.setBounds(right.removeFromTop(34));
    monoButton.setBounds(right.removeFromTop(34));
    latencyLabel.setBounds(right.removeFromTop(30));
    inputMeter.setBounds(right.removeFromLeft(110).reduced(4));
    outputMeter.setBounds(right.removeFromLeft(110).reduced(4));

    responseGraph.setBounds(area.reduced(8));
}

void OpenRefMonitorAudioProcessorEditor::timerCallback()
{
    inputMeter.setMeterState(processor.inputMeterState());
    outputMeter.setMeterState(processor.outputMeterState());
    latencyLabel.setText("Latency: " + juce::String(processor.getLatencySamples()) + " samples / "
                             + juce::String(1000.0 * processor.getLatencySamples() / processor.getSampleRate(), 1) + " ms",
                         juce::dontSendNotification);
}
