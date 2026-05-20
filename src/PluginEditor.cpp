#include "PluginEditor.h"

#include "Parameters.h"

OpenRefMonitorAudioProcessorEditor::OpenRefMonitorAudioProcessorEditor(OpenRefMonitorAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(1080, 650);

    title.setText("OpenRef Monitor", juce::dontSendNotification);
    title.setFont(juce::Font(26.0f, juce::Font::bold));
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

    modeBox.addItemList({ "Zero Latency", "Minimum Phase", "Linear Phase" }, 1);
    addAndMakeVisible(modeBox);

    translationBox.addItemList({ "Off", "Small Phone Speaker", "Laptop Speaker", "Basic Car Check", "Mono Midrange", "Bass-limited Check", "Bright Earbuds Check" }, 1);
    addAndMakeVisible(translationBox);

    for (auto* slider : { &amountSlider, &dryWetSlider, &outputGainSlider }) {
        slider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 90, 22);
        addAndMakeVisible(slider);
    }
    amountSlider.setName("Amount");
    dryWetSlider.setName("Dry/Wet");
    outputGainSlider.setName("Output");

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
    title.setBounds(top.removeFromLeft(280));
    correctionButton.setBounds(top.removeFromLeft(120).reduced(4));
    modeBox.setBounds(top.removeFromLeft(170).reduced(4));
    translationBox.setBounds(top.removeFromLeft(220).reduced(4));
    autoBypassButton.setBounds(top.removeFromLeft(230).reduced(4));

    warning.setBounds(area.removeFromBottom(32));
    attributionLabel.setBounds(area.removeFromBottom(26));

    auto right = area.removeFromRight(260).reduced(10);
    amountSlider.setBounds(right.removeFromTop(118));
    dryWetSlider.setBounds(right.removeFromTop(118));
    outputGainSlider.setBounds(right.removeFromTop(118));
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
