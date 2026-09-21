#include "PluginEditor.h"

static void styleSlider (juce::Slider& s)
{
    s.setSliderStyle (juce::Slider::LinearHorizontal);
    s.setTextBoxStyle (juce::Slider::TextBoxRight, false, 80, 24);
}

PanoramizatorAudioProcessorEditor::PanoramizatorAudioProcessorEditor (PanoramizatorAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setSize (520, 300);

    title.setText ("PANORAMIZATOR", juce::dontSendNotification);
    title.setFont (juce::FontOptions (24.0f).withStyle ("Bold"));
    title.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (title);

    styleSlider (timeSlider);
    styleSlider (leftSlider);
    styleSlider (rightSlider);
    addAndMakeVisible (timeSlider);
    addAndMakeVisible (leftSlider);
    addAndMakeVisible (rightSlider);

    timeSlider.setTextValueSuffix (" s");
    leftSlider.setTextValueSuffix (" L");
    rightSlider.setTextValueSuffix (" R");

    linkButton.setButtonText ("LINK");
    randomButton.setButtonText ("RANDOM");
    addAndMakeVisible (linkButton);
    addAndMakeVisible (randomButton);

    for (auto* label : { &timeLabel, &leftLabel, &rightLabel })
        addAndMakeVisible (*label);
    timeLabel.setText ("TIME", juce::dontSendNotification);
    leftLabel.setText ("LEFT", juce::dontSendNotification);
    rightLabel.setText ("RIGHT", juce::dontSendNotification);

    timeAttachment = std::make_unique<SliderAttachment> (processor.parameters, "TIME", timeSlider);
    leftAttachment = std::make_unique<SliderAttachment> (processor.parameters, "LEFT", leftSlider);
    rightAttachment = std::make_unique<SliderAttachment> (processor.parameters, "RIGHT", rightSlider);
    linkAttachment = std::make_unique<ButtonAttachment> (processor.parameters, "LINK", linkButton);
    randomAttachment = std::make_unique<ButtonAttachment> (processor.parameters, "RANDOM", randomButton);
}

void PanoramizatorAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);
    g.setColour (juce::Colours::white);
    g.drawRect (getLocalBounds().reduced (8), 1);
}

void PanoramizatorAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (24);
    title.setBounds (area.removeFromTop (42));
    area.removeFromTop (10);

    auto row = [&area] (juce::Label& label, juce::Slider& slider)
    {
        auto r = area.removeFromTop (42);
        label.setBounds (r.removeFromLeft (70));
        slider.setBounds (r);
        area.removeFromTop (4);
    };

    row (timeLabel, timeSlider);
    row (leftLabel, leftSlider);
    row (rightLabel, rightSlider);

    auto buttons = area.removeFromTop (42);
    linkButton.setBounds (buttons.removeFromLeft (100));
    buttons.removeFromLeft (12);
    randomButton.setBounds (buttons.removeFromLeft (110));
}
